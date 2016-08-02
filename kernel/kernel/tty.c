#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <kernel/vga.h>
#include <kernel/portio.h>
#include <stdlib.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void update_cursor(int row, int col)
 {
    unsigned short position= (unsigned short) ((row * 80) + col);
 
    write_port(0x3D4, 0x0F);
    write_port(0x3D5, (unsigned char)(position&0xFF));

    write_port(0x3D4, 0x0E);
    write_port(0x3D5, (unsigned char )((position>>8)&0xFF));

 }
void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
	update_cursor(0,0);
	log_info("init","Terminal");
	ok();
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}
void scroll(){
	for(int i = 1;i<VGA_HEIGHT;i++){
		for(int j=0;j < VGA_WIDTH;j++){
			size_t index = (i-1) * VGA_WIDTH + j;
			size_t index1 = i * VGA_WIDTH + j;
			terminal_buffer[index] = terminal_buffer[index1];
		}
	}
	for(int j=0;j < VGA_WIDTH;j++){
			size_t index = (VGA_HEIGHT-1) * VGA_WIDTH + j;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
}
void terminal_putchar(char c)
{
	if (c == '\n'){
		
		terminal_column = 0;
		if (terminal_row + 1 == VGA_HEIGHT)
			scroll();
		else terminal_row++;
		update_cursor(terminal_row,terminal_column);
		return;
	}
	
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if (terminal_row + 1 == VGA_HEIGHT)
			scroll();
		else
			terminal_row++;
	}
	update_cursor(terminal_row,terminal_column);
}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
void log_error(const char* data, const char* tag){
	terminal_color = make_color(COLOR_RED, COLOR_BLACK);	
	terminal_writestring(tag);
	terminal_writestring(" [error]: ");
	terminal_writestring(data);
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}
void error(const char* data){
	terminal_color = make_color(COLOR_RED, COLOR_BLACK);
	terminal_writestring(data);
	terminal_writestring(" ");
	abort();
}
void log_info(const char* data, const char* tag){
	terminal_color = make_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
	terminal_writestring(tag);
	terminal_writestring(" [info]: ");
	terminal_writestring(data);
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}
void ok(void){
	terminal_color = make_color(COLOR_GREEN, COLOR_BLACK);
	terminal_writestring(" ok.\n");
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}
void fail(void){
	terminal_color = make_color(COLOR_RED, COLOR_BLACK);	
	terminal_writestring(" fail.\n");
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}

static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		terminal_putchar((int) ((const unsigned char*) data)[i]);
}
static void print_int(unsigned long a){
	if (a / 10 != 0)
		print_int(a / 10);
	terminal_putchar((char) ('0' + (a % 10)));
}
static void print_hex(unsigned long a){
	if (a / 16 != 0)
		print_hex(a / 16);
	if (a % 16 < 10)
		terminal_putchar((char) ('0' + (a % 16)));
	else
		terminal_putchar((char) ('A' + (a % 16) - 10));
}
int printk(const char *restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
			print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
			incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if ( *format == 'd' )
		{
			format++;
			print_int(va_arg(parameters, unsigned int));
		}
		else if ( *format == 'X' )
		{
			format++;
			print("0x",2);
			print_hex(va_arg(parameters, unsigned int));
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}
