#include <stddef.h>
#include <stdint.h>

#include "obj/drivers/ports.h"
#include "obj/drivers/screen.h"
/* force to create kernel entry function */
void dummy_test_entrypoint() {
}

//vga ports header file
//Placed here instead of in a header file for compilation reasons
//unsigned char port_byte_in(unsigned short port);
//void port_byte_out(unsigned short port, unsigned char data);
//unsigned short port_word_in(unsigned short port);
//void port_word_out(unsigned short port, unsigned short data);

void main() {
	//Indexing for screen position:
	//Top left of the screen is position 0xb8000
	//Every odd bit controls the color of the previous position
	//Color codes for VGA can be found here: http://www.osdever.net/bkerndev/Docs/printing.htm

	//Note to self - memory+0xa0 is in the same column
	//Each row contains a0 characters
	
	//VGA ports 0x3d4 and 0x3d5 handle cursor data
	//14 contains the high byte and 15 contains the low byte for cursor position
	//Example usage of the ports:
	port_byte_out(REG_SCREEN_CTRL, 14);
	int position = port_byte_in(REG_SCREEN_DATA);
	position = position << 8;		//High byte needs to be shifted
	port_byte_out(REG_SCREEN_CTRL, 15);
	position += port_byte_in(REG_SCREEN_DATA);//Low byte just needs to be added
	
	//Example of the finalized print functionality
	//2 important print functions: print_at(String, Position), print_at_color(String, Position, VGA Color Code)
	print_at_color("BASA-OS", VIDEO_ADDRESS+0xa00, GREEN);
}

int length(char* str);

//This is bad practice but the makefile wasn't configured to include header files and I'm not sure
//how to fix compilation in that way
unsigned char port_byte_in(unsigned short port)
{
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

unsigned short port_word_in(unsigned short port)
{
	unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

//Vars are already in registers, so no "=" is needed
void port_byte_out(unsigned short port, unsigned char data)
{
	__asm__("out %%al, %%dx" : "=a" (data) : "d" (port));
}

void port_word_out(unsigned short port, unsigned short data)
{
	__asm__("out %%ax, %%dx" : "=a" (data) : "d" (port));
}

//Custom print function that takes in a string str and prints it character by character starting from position pos
void print_at(char* str, int pos)
{
	for (int x = 0; x < length(str); ++x)
	{
		char* temp_mem = (char*) pos + 0x2*x;
		*temp_mem = str[x];
	}
}

//Custom print function that takes in a string str, screen position pos, and a color code and prints it at pos
//Ex usage: print_at_color("BASA-OS", 0xb8a00, 0x0f)
//For color codes: First bit is background color, second is foreground color
//eg. 0x0f is white text on black background
//VGA color codes: http://www.osdever.net/bkerndev/Docs/printing.htm
//0 - Black
//1 - Blue
//2 - Green
//4 - Red
//5 - Magenta
//e - Yellow
//f - White
void print_at_color(char* str, int pos, unsigned short color)
{
	char* vga = pos;
	for (int x = 0; x < length(str); ++x)
	{
		vga[2*x] = str[x];
		vga[2*x+1] = color;
	}
}

void print_vertical(char* str, int pos)
{
	for (int x = 0; x < length(str); ++x)
	{
		char* temp_mem = (char*) pos + 0xa0*x;
		*temp_mem = str[x];
	}
}

//String length function
int length(char* str)
{
	int x = 0;
	while (str[x] != '\0')
		x++;
	return x;
}

uint16_t vga_entry(char c, uint8_t color) {
  return ((uint16_t) c) | (((uint16_t) color) << 8);
}

static const size_t VGA_HEIGHT = 24;
static const size_t VGA_WIDTH = 80;

size_t term_row = 0;
size_t term_col = 0;
uint8_t term_color = 7; // Gray on black
uint16_t* term_buf = (uint16_t*) 0xB8000; // Location of VGA text mode buffer

void term_init() {
  for(size_t y = 0; y < VGA_HEIGHT; y++) {
    for(size_t x = 0; x < VGA_HEIGHT; x++) {
      const size_t index = y * VGA_WIDTH + x;
      term_buf[index] = vga_entry('x', term_color);
    }
  }
}

void kernel_main() {
  term_init();
}