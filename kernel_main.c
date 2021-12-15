#include "lib_asm.h"
#include "lib_c.h"
#include "string.h"
#include "FAT.h"

char inp[100];
int cntr = 0;
char dir[] = "dir\n";
char demo[] = "demo\n";


unsigned char *video = (unsigned char *)0xF0000000; //Video array to store information of the pixels
int height; //the y-coordinte of the top left corner of a character
int width; //the x-coordinte of the top left corner of a character

void isr_default(unsigned char num, ISR_Stack_Frame isf);
void isr_pit(unsigned char num, ISR_Stack_Frame isf);
void isr_keyb(unsigned char num, ISR_Stack_Frame isf);
void setpixel(int x, int y, int b, int g, int r);
void drawtext(int charnum);
void drawline(int x1, int y1, int x2, int y2, int b, int g, int r);
void drawcircle(int xc, int yc, int radius, int b, int g, int r);
int sqrt(int temps);
void fillrect(int x, int y, int w, int h, int b, int g, int r);
void cleanscreen();

unsigned char kbdus[128] =
{
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`', 0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/', 0,				/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0, 0, 0, 0, 0, 0, 0, 0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0, 0, 0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};

int part_start_lba;
int part_length;


int main(void) {
	part_start_lba = 33543720;
	part_length = 2*8385930;

	char buf, *addr, checksum;
	int i, count;

	printss("\nMiniOS32 Kernel is Active!!!\n");
	printss("Copyright (C) 2014\n");
	printss("Enter \"?\" for help\n");
	printss("Boot ROM date = ");
	printsss((char *)0xFFFF5, 8);
	printss("\nEIP = ");
	printhex(get_eip(), 8);

	char new_gdt[24] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00,
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00 };

	GDTR gdtr;
	gdtr.limit = sizeof(new_gdt)-1;
	gdtr.base = new_gdt;
	lgdt(&gdtr, 0x0008, 0x0010);

	// Prevent an unfortunately-timed IRQ from messing up the whole works
	irq_disable();

	// Setup IDT and all associated supports
	idt_install();

	// Install some elementary ISRs
	idt_set_gate(0x0F, isr_default, 0x0008, 0x8E);
	idt_set_gate(0x20, isr_pit, 0x0008, 0x8E);
	idt_set_gate(0x21, isr_keyb, 0x0008, 0x8E);

	// ...and respond to IRQs again
	irq_enable();

	//Draw an equilateral triangle by drawing three sides onto the screen
	drawline(640, 250, 390, 683, 255, 255, 255); 
	drawline(640, 250, 890, 683, 255, 255, 255); 
	drawline(390, 683, 890, 683, 255, 255, 255); 
	//Draw a vertial line passing through the triangle
	drawline(640, 250, 640, 683, 255, 255, 255); 
	//Draw the inscribed circle of the triangle
	drawcircle(640, 538, 145, 255, 255, 255);
	
	//Draw two blue banners with greetings written inside
	fillrect(0, 0, 1280, 28, 255, 0, 0);
	fillrect(0, 996, 1280, 28, 255, 0, 0);

	//printss("In\nHEX: 0x");
	//printhex(FATInitialize(), 8);
	//printss("\nOut\n");
	
	width = 480;
	height = 6;
	drawtext('W');
	drawtext('E');
	drawtext('L');
	drawtext('C');
	drawtext('O');
	drawtext('M');
	drawtext('E');
	drawtext(' ');
	drawtext('T');
	drawtext('O');
	drawtext(' ');
	drawtext('T');
	drawtext('H');
	drawtext('E');
	drawtext(' ');
	drawtext('O');
	drawtext('S');

	
	//Set the cursor for keyboard input 
	width = 0;
	height = 35;
	drawtext('_');
	//printss("bottom");
	while (1) {

		printss("\n>");
		
		//printss("Made it to the top(now we're here)");
		
		//getcc(1);
		
		//printss("Made it past getcc(now we're here)");
		
		//lowercase('A');
		
		//printss("Made it past lowercase (now we're here)");

		#define TTY       0x03F8
		#define LSR	      5

		
		char c;
		//printss("Lots starting!\n");

		while ( ( inb( TTY + LSR ) & 0x01 ) == 0x00 )
			{
				//printss("Very loop, much dizzy!\n");
			}
		//printss("WOW!\n");
		c = inb( TTY );
		//printss("Much success!\n");
		putcc( c );
		//printss("Very beauty!\n");
		buf = lowercase(c);
		#undef TTY
		#undef LSR
		//printss("Made it past lowercase getcc (now we're here)");

		if (buf == 'b') {

			printss("\nRebooting!!\n");
			reset();

		}

		else if (buf == 'c') {

			if (gethex((void *)&addr, 8, -1) != 0)
				continue;

			// The line below treats addr as a pointer to a function
			((void(*) ()) addr)();

		}

		else if (buf == 'd') {

			if (gethex((void *)&addr, 8, -1) != 0)
				continue;

			if (gethex((void *)&count, 8, -1) != 0)
				continue;

			count += (unsigned long)addr & 0x0000000F;
			addr = (char *)((unsigned long)addr & 0xFFFFFFF0);
			checksum = 0;

			while (count > 0) {

				printss("\n");
				printhex((unsigned long)addr, 8);
				printss(" ");

				for (i = 0; i < 16; i++, addr++) {

					printhex(*addr, 2);
					printss(" ");

				}

				addr -= 16;

				for (i = 0; i < 16; i++, addr++) {

					printsss(addr, 1);
					checksum += *addr;

				}

				count -= 16;

			}

			printss("\nChecksum = ");
			printhex(checksum, 2);

		}

		else if ((buf == 'f') || (buf == 's')) {

			if (gethex((void *)&addr, 8, (buf == 's') ? 0 : -1) != 0)
				continue;

			addr = (char *)((unsigned long)addr & 0xFFFFFFF0);

			for (i = 0;; i++, addr++) {

				if ((i % 16 == 0) && (buf != 's')) {

					printss("\n");
					printhex((unsigned long)addr, 8);

				}

				if (gethex((void *)&count, 2, (buf == 's') ? 0 : -1) != 0)
					break;

				*addr = *((char *)&count);

			}

		}

		else if (buf == 'i') {

			if (gethex((void *)&addr, 4, -1) != 0)
				continue;

			printss("\n");
			printhex(inb((unsigned long)addr), 2);

		}

		else if (buf == 'o') {

			if (gethex((void *)&addr, 4, -1) != 0)
				continue;

			if (gethex((void *)&count, 2, -1) != 0)
				continue;

			outb((unsigned long)addr, count);

		}

		else if (buf == 'm') {

			if (gethex((void *)&addr, 8, -1) != 0)
				continue;

			printss("\n");
			printhex(*addr, 2);

			if (gethex((void *)&count, 2, -1) != 0)
				continue;

			*addr = count;

		}

		else if (buf == 'w') {

			if (gethex((void *)&addr, 2, -1) != 0)
				continue;

			software_interrupt((unsigned char)(unsigned long)addr);

		}

		else if (buf == 'x') {

			/*
			printss( "\nisr_template_start = " );
			printhex( ( unsigned long ) &isr_template_start, 8 );
			printss( "\nisr_template_error_start = " );
			printhex( ( unsigned long ) &isr_template_error_start, 8 );
			printss( "\nisr_template_error_end = " );
			printhex( ( unsigned long ) &isr_template_error_end, 8 );
			printss( "\nisr_template_num = " );
			printhex( ( unsigned long ) &isr_template_num, 8 );
			printss( "\nisr_template_cfunc_start = " );
			printhex( ( unsigned long ) &isr_template_cfunc_start, 8 );
			printss( "\nisr_template_cfunc_end = " );
			printhex( ( unsigned long ) &isr_template_cfunc_end, 8 );
			printss( "\nisr_template_end = " );
			printhex( ( unsigned long ) &isr_template_end, 8 );
			printss( "\nlength = " );
			printhex( ( unsigned long ) ( &isr_template_end - &isr_template_start ), 8 );
			*/

			// Prevent an unfortunately-timed IRQ from messing up the whole works
			irq_disable();

			// Setup IDT and all associated supports
			idt_install();

			// Install some elementary ISRs
			idt_set_gate(0x0F, isr_default, 0x0008, 0x8E);
			idt_set_gate(0x20, isr_pit, 0x0008, 0x8E);

			// ...and respond to IRQs again
			irq_enable();

		}

		else if (buf == 'y') {

			IDTR idtr;

			sidt(&idtr);

			printss("\nidtr.limit=");
			printhex(idtr.limit, 4);
			printss("\nidtr.base=");
			printhex((unsigned long)idtr.base, 8);

		}

		else if (buf == 'z') {

			for (i = 0; i < 256; i++) {

				if (i % 4 == 0)
					printss("\n");
				else
					printss("    ");
				printhex(i, 2);
				printss(": ");
				printhex(irq_count[i], 8);

			}

		}
		else if (buf == 'p') {

			printss("\nInitializing... 0x");
			printhex(FATInitialize(), 8);
			printss("\n");

		}

		else if (buf == '?') {

			printss("\nInternal commands:");
			printss("\nb                           - reboot");
			printss("\nc <addr>                    - call function");
			printss("\nd <addr> <count>            - dump range of memory");
			printss("\nf <addr>                    - fill range of memory");
			printss("\ni <port>                    - read byte from I/O port");
			printss("\nm <addr>                    - modify memory location");
			printss("\no <port> <byte>             - write byte to I/O port");
			printss("\ns <addr>                    - \"silent\" fill (same as f without echo)");

		}
		
		printss("Made it to the bottom (now we're here)");

	}

	return 0;

}

void isr_pit(unsigned char num, ISR_Stack_Frame isf) {

	// Send EOI to PIC
	outb(0x20, 0x20);

}

void isr_default(unsigned char num, ISR_Stack_Frame isf) {

	unsigned long *y = (unsigned long *)&isf;

	int i;

	printss("\nsizeof( ISR_Stack_Frame ) = ");
	printhex(sizeof(ISR_Stack_Frame), 8);
	printss("\nnum = ");
	printhex(num, 2);
	printss("\nisf.gs = ");
	printhex(isf.gs, 4);
	printss("\nerror code = ");
	printhex(isf.error_code, 2);
	printss("\nisf.eip:");
	printhex(isf.eip, 8);
	printss("\nisf.cs = ");
	printhex(isf.cs, 4);
	printss("\nisf.eflags = ");
	printhex(isf.eflags, 8);
	printss("\nStack Frame:");

	for (i = 0; i < 32; i++) {

		printss("\n");
		printhex(i, 2);
		printss(" ");
		printhex((unsigned long)y, 8);
		printss(": ");
		printhex(*y, 8);

		y++;

	}

	reset();

}

void isr_keyb(unsigned char num, ISR_Stack_Frame isf) {
	//unsigned char scancode = inb(0x60);///////
	unsigned char buf = inb(0x0060);

	// Key pressed
	if ((buf & 0x80) == 0)
	{
		width = width - 16;
		drawtext(-1);
		width = width - 16;
		
		inp[cntr] = kbdus[buf];
		putcc (inp[cntr]);
		//drawtext(kbdus[buf]);
		drawtext(inp[cntr]);
		//_printf("\n%d\n", cntr);
		if (inp[cntr] == '\n') // replace with enter
		{
			_printf("ENTERKEY\n");
			int k = 0;
			for (k = 0; k < cntr; k++)
			{
				drawtext(inp[k]);
				putcc (inp[k]);
				_printf("\n");
				printsss(inp, 10);
			}
			
			
			if (strcmp (dir, inp) == 0)
			{
				//FATInitialize();
				char* fileContents;
				directory_entry_t fileMeta;
				getFile("C:\\", &fileContents, &fileMeta, 1);
				directoryList(GET_CLUSTER_FROM_ENTRY(fileMeta), NULL, FALSE);
				
				
				
				_printf("CORRECT! IMPLEMENT DIR CODE HERE\n");
				drawtext(' ');
				drawtext('O');
				drawtext(' ');
				int l = 0;
				for (l = 0; l < cntr; l++)
				{
					inp[l] = '\0';
				}
			}
			
			else if (strcmp (demo, inp) == 0)
			{
				height = 50;
				width = 5;
				d_printss("\n\n\nBeginning FAT code demonstration... \n");
				d_printss("The FAT code first requires retrieval of data from the disk to figure out the FAT type.\nThis is handled by FATInitialize(), which was called at the beginning of execution.\nNote: only FAT32 is currently supported.\n");
				d_printss("Sectors Per Cluster ( from FATInitialize() ): 0x");
				d_printhex((short)(bootsect.sectors_per_cluster), 2);
				
				d_printss("\n\nThe function FATRead() is capable of reading entries in the main FAT table and returning the number of the next cluster in a file's chain.\n");
				d_printss("The function FATWrite() writes cluster numbers to the main FAT table (but not the secondary FAT table).\n");
				d_printss("The function allocateFreeFAT() will find a free cluster, and write 0xFFFFFFF8 (END_CLUSTER_32) to it, to indicate the block's usage.\n");
				unsigned int a_cluster = allocateFreeFAT();
				d_printss("allocateFreeFAT() found the cluster 0x");
				d_printhex(a_cluster, 8);
				d_printss(" for our demonstration. FATRead() reads the value 0x");
				d_printhex(FATRead(a_cluster), 8);
				d_printss(" from that cluster.\n");
				d_printss("Now using FATWrite(), we will write 0x01234567 to that cluster entry.\n");
				if (FATWrite(a_cluster, 0x1234567) != 0)
					{
						d_printss("This demonstration encountered an error. Aborting...\n");
						goto end_of_demo;
					}
				d_printss("FATRead() reads ");
				d_printhex(FATRead(a_cluster), 8);
				d_printss(" from that cluster entry, as expected.\n");
				FATWrite(a_cluster, FREE_CLUSTER_32); //reset the demo's damage.
				
				d_printss("clusterRead() and clusterWrite() will read and write data from a FAT cluster. Currently, in the cluster we found earlier, this is the first 20 characters found in there: \n");
				clusterRead(a_cluster, 0);
				d_printsss((char*)DISK_READ_LOCATION, 20);
				d_printss("Now to write \"clusterWrite() works!\" to disk... Filling memory location with zeros...\n New contents: ");
				strcpy((char*)DISK_READ_LOCATION, "clusterWrite() works!");
				clusterWrite(DISK_READ_LOCATION, strlen("clusterWrite() works!"), 0, a_cluster);
				memset(DISK_READ_LOCATION, 0, 30);
				clusterRead(a_cluster, 0);
				d_printss((char*)DISK_READ_LOCATION);
				d_printss("\n");
		
				char test[13];
				strcpy (test, "test.txt");
				d_printss("convertToFATFormat() converts test.txt into ");
				d_printss(convertToFATFormat(test));
				d_printss("\n");
				d_printss("testIfFATFormat() checks to ensure a value obeys FAT specifications. The converted file name above is flagged as ");
				if (testIfFATFormat(test) == 0)
					d_printss("valid");
				else
					d_printss("invalid");
				d_printss (" by testIfFATFormat().\nconvertFromFATFormat converts that back to ");
				char output[13];
				convertFromFATFormat(test, output);
				d_printss(output);
				d_printss("\n");
		
				//d_printhex(testIfFATFormat("DOCUME~1   "), 2);
		
				d_printss("the function getFile() utilitizes all the functions above, plus directorySearch(which uses the functions above to look for a file in a directory) to display the contents of a file.\nPlease make sure a file named \"test.txt\" is in the \"Documents and Settings\" folder, to demonstrate it's ability to also search subdirectories.\nStarting getFile...\n\n");
		
				char* fileContents;
				directory_entry_t filemeta; 
				getFile("C:\\DOCUME~1\\test.txt", &fileContents, &filemeta, 1);
		
				d_printss(fileContents);
				d_printss("\nA shout-out to strcpy(), strncpy(), strncmp(), strcmp(), and the read and write disk drivers (all written by me) for making this possible!\n");
				
				end_of_demo:
				d_printss ("\n\nThis concludes the demonstration.\n");
		

				int l = 0;
				for (l = 0; l < cntr; l++)
				{
					inp[l] = '\0';
				}
				
			}
			
			else 
			{
			int j = 0;
				for (j = 0; j < cntr; j++)
				{
					inp[j] = '\0';
				}
			}
			
			cntr = 0;
		}
		
		else
		{
			cntr++;
		}
		
		//cntr++;
		width = width - 2;
		drawtext('_');
		width = width + 2;
	}

	// Key released
	else 
	{
		//cntr++;
	}

	// Send EOI to PIC
	outb(0x20, 0x20);

}


void setpixel(int x, int y, int b, int g, int r) {//Set a specific pixel on screen to white
	
	unsigned int offset = (x + y * 1280) * 4; //Finding the location of the pixel in the video array

	video[offset] = b;   //Setting the color using BGR
	video[offset + 1] = g;
	video[offset + 2] = r;
	video[offset + 3] = 0;

	return;
}

void drawtext(int charnum){//draw a single character using a 8*8 pattern
	// Store hex numbers representing the pattern for characters (8 numbers per character) into an array
	char textarray[2057] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,
		0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E,
		0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00,
		0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00,
		0x38, 0x7C, 0x38, 0xFE, 0xFE, 0xD6, 0x10, 0x38,
		0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x10, 0x38,
		0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,
		0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,
		0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,
		0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,
		0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,
		0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18,
		0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,
		0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0,
		0x18, 0xDB, 0x3C, 0xE7, 0xE7, 0x3C, 0xDB, 0x18,
		0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00,
		0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00,
		0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18,
		0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
		0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00,
		0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78,
		0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00,
		0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,
		0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x30, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x30, 0x00, 0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00, 0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00, 0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00, 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00, 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, 0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x00, 0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00, 0x78, 0xCC, 0x0C, 0x38, 0x60, 0xCC, 0xFC, 0x00, 0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00, 0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00, 0xFC, 0xC0, 0xF8, 0x0C, 0x0C, 0xCC, 0x78, 0x00, 0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00, 0xFC, 0xCC, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00, 0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x00, 0x78, 0xCC, 0xCC, 0x7C, 0x0C, 0x18, 0x70, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60, 0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x78, 0xCC, 0x0C, 0x18, 0x30, 0x00, 0x30, 0x00, 0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x78, 0x00, 0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00, 0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00, 0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00, 0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00, 0xFE, 0x62, 0x68, 0x78, 0x68, 0x62, 0xFE, 0x00, 0xFE, 0x62, 0x68, 0x78, 0x68, 0x60, 0xF0, 0x00, 0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3E, 0x00, 0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00, 0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00, 0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x00, 0xF0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x00, 0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00, 0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00, 0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0xDC, 0x78, 0x1C, 0x00, 0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00, 0x78, 0xCC, 0x60, 0x30, 0x18, 0xCC, 0x78, 0x00, 0xFC, 0xB4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00, 0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00, 0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x78, 0x00, 0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x00, 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00, 0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x00, 0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00, 0x1C, 0x0C, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, 0x38, 0x6C, 0x60, 0xF0, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, 0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x00, 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0xCC, 0xFE, 0xFE, 0xD6, 0xC6, 0x00, 0x00, 0x00, 0xF8, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0, 0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E, 0x00, 0x00, 0xDC, 0x76, 0x66, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x00, 0x10, 0x30, 0x7C, 0x30, 0x30, 0x34, 0x18, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00, 0x00, 0x00, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x00, 0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, 0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00, 0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x00, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x00, 0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x18, 0x0C, 0x78, 0x00, 0xCC, 0x00, 0xCC, 0xCC, 0xCC, 0x7E, 0x00, 0x1C, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, 0x7E, 0xC3, 0x3C, 0x06, 0x3E, 0x66, 0x3F, 0x00, 0xCC, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x7E, 0x00, 0xE0, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x7E, 0x00, 0x30, 0x30, 0x78, 0x0C, 0x7C, 0xCC, 0x7E, 0x00, 0x00, 0x00, 0x78, 0xC0, 0xC0, 0x78, 0x0C, 0x38, 0x7E, 0xC3, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, 0xCC, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, 0xE0, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, 0xCC, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 0x7C, 0xC6, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00, 0xE0, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 0xC6, 0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0xC6, 0x00, 0x30, 0x30, 0x00, 0x78, 0xCC, 0xFC, 0xCC, 0x00, 0x1C, 0x00, 0xFC, 0x60, 0x78, 0x60, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0x0C, 0x7F, 0xCC, 0x7F, 0x00, 0x3E, 0x6C, 0xCC, 0xFE, 0xCC, 0xCC, 0xCE, 0x00, 0x78, 0xCC, 0x00, 0x78, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0xCC, 0x00, 0x78, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0xE0, 0x00, 0x78, 0xCC, 0xCC, 0x78, 0x00, 0x78, 0xCC, 0x00, 0xCC, 0xCC, 0xCC, 0x7E, 0x00, 0x00, 0xE0, 0x00, 0xCC, 0xCC, 0xCC, 0x7E, 0x00, 0x00, 0xCC, 0x00, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, 0xC3, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x00, 0xCC, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x18, 0x18, 0x7E, 0xC0, 0xC0, 0x7E, 0x18, 0x18, 0x38, 0x6C, 0x64, 0xF0, 0x60, 0xE6, 0xFC, 0x00, 0xCC, 0xCC, 0x78, 0xFC, 0x30, 0xFC, 0x30, 0x30, 0xF8, 0xCC, 0xCC, 0xFA, 0xC6, 0xCF, 0xC6, 0xC7, 0x0E, 0x1B, 0x18, 0x3C, 0x18, 0x18, 0xD8, 0x70, 0x1C, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x7E, 0x00, 0x38, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x1C, 0x00, 0x78, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x1C, 0x00, 0xCC, 0xCC, 0xCC, 0x7E, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0xCC, 0xCC, 0xCC, 0x00, 0xFC, 0x00, 0xCC, 0xEC, 0xFC, 0xDC, 0xCC, 0x00, 0x3C, 0x6C, 0x6C, 0x3E, 0x00, 0x7E, 0x00, 0x00, 0x38, 0x6C, 0x6C, 0x38, 0x00, 0x7C, 0x00, 0x00, 0x30, 0x00, 0x30, 0x60, 0xC0, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x0C, 0x0C, 0x00, 0x00, 0xC3, 0xC6, 0xCC, 0xDE, 0x33, 0x66, 0xCC, 0x0F, 0xC3, 0xC6, 0xCC, 0xDB, 0x37, 0x6F, 0xCF, 0x03, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x33, 0x66, 0xCC, 0x66, 0x33, 0x00, 0x00, 0x00, 0xCC, 0x66, 0x33, 0x66, 0xCC, 0x00, 0x00, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0xDB, 0x77, 0xDB, 0xEE, 0xDB, 0x77, 0xDB, 0xEE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x36, 0x36, 0x36, 0x00, 0x00, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x36, 0x36, 0xF6, 0x06, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0xFE, 0x06, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x06, 0xFE, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36, 0xFE, 0x00, 0x00, 0x00, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0xF7, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xF7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x36, 0x36, 0xF7, 0x00, 0xF7, 0x36, 0x36, 0x36, 0x18, 0x18, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x3F, 0x00, 0x00, 0x00, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x36, 0x36, 0x36, 0x18, 0x18, 0xFF, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xDC, 0xC8, 0xDC, 0x76, 0x00, 0x00, 0x78, 0xCC, 0xF8, 0xCC, 0xF8, 0xC0, 0xC0, 0x00, 0xFC, 0xCC, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0xFE, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x00, 0xFC, 0xCC, 0x60, 0x30, 0x60, 0xCC, 0xFC, 0x00, 0x00, 0x00, 0x7E, 0xD8, 0xD8, 0xD8, 0x70, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0xC0, 0x00, 0x76, 0xDC, 0x18, 0x18, 0x18, 0x18, 0x00, 0xFC, 0x30, 0x78, 0xCC, 0xCC, 0x78, 0x30, 0xFC, 0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0x6C, 0x38, 0x00, 0x38, 0x6C, 0xC6, 0xC6, 0x6C, 0x6C, 0xEE, 0x00, 0x1C, 0x30, 0x18, 0x7C, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x7E, 0xDB, 0xDB, 0x7E, 0x00, 0x00, 0x06, 0x0C, 0x7E, 0xDB, 0xDB, 0x7E, 0x60, 0xC0, 0x38, 0x60, 0xC0, 0xF8, 0xC0, 0x60, 0x38, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0xFC, 0x00, 0xFC, 0x00, 0xFC, 0x00, 0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0xFC, 0x00, 0x60, 0x30, 0x18, 0x30, 0x60, 0x00, 0xFC, 0x00, 0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0xFC, 0x00, 0x0E, 0x1B, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70, 0x30, 0x30, 0x00, 0xFC, 0x00, 0x30, 0x30, 0x00, 0x00, 0x76, 0xDC, 0x00, 0x76, 0xDC, 0x00, 0x00, 0x38, 0x6C, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x0F, 0x0C, 0x0C, 0x0C, 0xEC, 0x6C, 0x3C, 0x1C, 0x78, 0x6C, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x70, 0x18, 0x30, 0x60, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	//getFile("C:\\v11.txt", &textarray, NULL, 1);

	int i; //column number in the 8*8 pattern
	int k; //row number in the 8*8 pattern
	int w;//horizontal position of the pixel in the 8*8 pattern
	int h;//vertical position of the pixel in the 8*8 pattern
	
	//charnum = charnum + 1;
	//Moving the cursor to the next line when we reached the end of the existing line
	if (width > 1260){
		width = 0;
		height = height + 16;
	}
	
	if (width < -2 && height >34){
		width=1250;
		height = height - 16;
	}
	
	if (charnum == -1){
		fillrect(width, height, 16, 16, 0, 0, 0);
		width=width+16;
	}

	else if (charnum==10){//Enter
		drawtext(-1);
		width = 0;
		height = height + 16;
	}

	else if (charnum== 8){ // backspace
		drawtext(-1);
		width = width - 32;
		drawtext(-1);
		drawtext(-1);
		width = width - 32;
		//drawtext('_');
	}

	else {
		//Draw the character with 16*16 pixels
		for (k = 0; k < 8; k++){
			for (i = 0; i < 8; i++){
				if (textarray[charnum * 8 + k] & (0x01 << i)){
					for (w = 2 * (8 - i); w < 2 * (8 - i) + 2; w++){
						for (h = 2 * k; h < 2 * k + 2; h++){
							setpixel(w + width, h + height, 255, 255, 255);
						}
					}
				}
			}
		}
		width = width + 16;//Move to draw the next character
	}
}
void drawline(int x1, int y1, int x2, int y2, int b, int g, int r){//Draw a line on the screen

	int dx = x2 - x1;      // the horizontal distance of the line
	int dy = y2 - y1;      // the vertical distance of the line
	int dyabs = dy;		   // the absolute value of the two distances
	int dxabs = dx;
	int px = 0;         // x-coordinate of location of the pixel
	int py = 0;		    // y-coordinate of location of the pixel
	int temp;		    // used in comparison

	//Find the absolute values of distances
	if (dx < 0)
		dxabs = -dx;

	if (dy < 0)
		dyabs = -dy;


	if (dxabs >= dyabs){ //If the line is more horizontal than vertical */

		if (x1>x2){ // Convering so that x1 is always the smaller value between x1 and x2
			temp = x2;
			x2 = x1;
			x1 = temp;
		}

		for (px = x1; px <= x2; px++){ //Draw line
			int tempy = dy*(px - x1);
			py = y1 + tempy / dxabs;
			setpixel(px, py, b, g, r);
		}
	}

	else {/* the line is more vertical than horizontal */

		if (y1>y2){ // Convering so that y1 is always the smaller value between y1 and y2
			temp = y2;
			y2 = y1;
			y1 = temp;
		}

		for (py = y1; py <= y2; py++){ //Draw line
			int tempx = dx*(py - y1);
			px = x1 + tempx / dyabs;
			setpixel(px, py, b, g, r);
		}
	}
}

void drawcircle(int xc, int yc, int radius, int b, int g, int r){//Draw a circle on the screen

	int px;//x-coordinate of the pixel
	int py;//y-coordinate of the pixel
	int py1;//another y-coordinate of the pixel
	int py2;//another y-coordinate of the pixel
	int px1;//another x-coordinate of the pixel
	int px2;//another x-coordinate of the pixel
	int temps;// the square of a number

	for (px = xc + radius; px >= xc; px--){ // Draw the right half of the circle
		temps = radius*radius - (px - xc)*(px - xc); //Square of the distance from y-coordinate of the pixel to y-coordinate of the center
		py1 = yc - sqrt(temps);//possible y-coordinates of the pixel
		py2 = yc + sqrt(temps);
		setpixel(px, py1, b, g, r);
		setpixel(px, py2, b, g, r);
	}

	for (px = xc; px >= xc - radius; px--){ // Draw the left half of the circle
		temps = radius*radius - (xc - px)*(xc - px);
		py1 = yc - sqrt(temps);
		py2 = yc + sqrt(temps);
		setpixel(px, py1, b, g, r);
		setpixel(px, py2, b, g, r);
	}


	for (py = yc - radius; py <= yc; py++){// Draw the down half of the circle
		temps = radius*radius - (py - yc)*(py - yc);//Square of the distance from x-coordinate of the pixel to x-coordinate of the center
		px1 = xc - sqrt(temps); //possible x-corrdinates of the pixdel
		px2 = xc + sqrt(temps);
		setpixel(px1, py, b, g, r);
		setpixel(px2, py, b, g, r);
	}

	for (py = yc; py <= yc + radius; py++){ // Draw the up half of the circle
		temps = radius*radius - (yc - py)*(yc - py);
		px1 = xc - sqrt(temps);
		px2 = xc + sqrt(temps);
		setpixel(px1, py, b, g, r);
		setpixel(px2, py, b, g, r);
	}
}

void fillrect(int x, int y, int w, int h, int b, int g, int r){//Draw a filled rectangle
	int i, j;
	unsigned int offset = (x + y * 1280) * 4; //Finding the location of the pixel in the video array
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {//color each line
			video[offset + j * 4] = b;
			video[offset + j * 4 + 1] = g;
			video[offset + j * 4 + 2] = r;
			video[offset + j * 4 + 3] = 0;
		}
		offset += 5120;//switch to the beginnering of next line
	}
}

int sqrt(int square){//Find the square root of a number
	int i;
	for (i = 0; i < square; i++){
		if (i*i <= square && (i + 1)*(i + 1) >= square)
			return i;
	}
}

void cleanscreen(){
		fillrect(0,35, 1280,996, 255,255,255);
}



// Read sector(s) from booted partition to 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_read_o(unsigned long sector_offset, unsigned char num_blocks, unsigned long readLocationOffset) {

	// Bounds check; only read sectors on the booted partition
	if (sector_offset + num_blocks > part_length)
		return -1;

	//Size check; only read blocks if the resulting size read will still be between 0x40000 and 0x80000
	//if (readLocationOffset + num_blocks * 512 > (0x80000 - DISK_READ_LOCATION))
		//return -1;

	// Sanity check; BIOS int13h can only read up to 0x7F blocks
	if (num_blocks > 0x7F)
		return -1;

	sector_offset = sector_offset + part_start_lba;
   
   //Implementing LBA28
 
   //Send out LBA and read instructions to disk
   outb(0x1F1, NULL);
   outb(0x1F2, num_blocks);
   outb(0x1F3, (unsigned char)sector_offset);
   outb(0x1F4, (unsigned char)(sector_offset >> 8));
   outb(0x1F5, (unsigned char)(sector_offset >> 16));
   outb(0x1F6, 0xE0 | (0 << 4) | ((sector_offset >> 24) & 0x0F));
   outb(0x1F7, 0x20);
  
   unsigned char iterator = 0;
   unsigned short* buffer = DISK_READ_LOCATION + readLocationOffset;

   for (iterator = 0; iterator < num_blocks; iterator++)
   {
		while (!(inb(0x1F7) & 0x08)) {
			}
		
		// for read:

		unsigned short idx = 0;
		//read in bytes from disk
		for (idx = 0; idx < 256; idx++)
		{
			buffer[idx] = inw(0x1F0);
		}
		buffer += idx;
	}
   return 0;
   
}

// Write sector(s) to booted partition from 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_write_o(unsigned long sector_offset, unsigned char num_blocks, unsigned long writeLocationOffset) {
	// Bounds check; only write sectors on the booted partition
	if (sector_offset + num_blocks > part_length)
		return -1;

	//Size check; only write blocks if the resulting size written will still be between 0x40000 and 0x80000
	//if (writeLocationOffset + num_blocks * 512 > (0x80000 - DISK_WRITE_LOCATION))
		//return -1;

	// Sanity check; BIOS int13h can only write up to 0x7F blocks
	if (num_blocks > 0x7F)
		return -1;

	sector_offset = sector_offset + part_start_lba;
   
   //Implementing LBA28
   
   //Send out LBA and write instructions to disk
   outb(0x1F1, NULL);
   outb(0x1F2, num_blocks);
   outb(0x1F3, (unsigned char)sector_offset);
   outb(0x1F4, (unsigned char)(sector_offset >> 8));
   outb(0x1F5, (unsigned char)(sector_offset >> 16));
   outb(0x1F6, 0xE0 | (0 << 4) | ((sector_offset >> 24) & 0x0F));
   outb(0x1F7, 0x30);
   
   
   unsigned char iterator = 0;
   unsigned short* buffer = DISK_WRITE_LOCATION + writeLocationOffset;

   for (iterator = 0; iterator < num_blocks; iterator++)
   {
		//check for errors before writing
		while (1) {
			if ((inb(0x1F7) & 0x01) == 0x01)
				printss ("error1\n");
			else if ((inb(0x1F7) & 0x40) != 0x40)
				printss ("Not ready!\n");
			else if ((inb(0x1F7) & 0x80) == 0x80)
				printss ("Busy!\n");
			else if ((inb(0x1F7) & 0x08) != 0x08)
				printss ("Have no data/not ready to recieve PIO!\n");
			else
			break;
		}

		unsigned short idx = 0;
		//send out data to disk
		for (idx = 0; idx < 256; idx++)
		{
			outw(0x1F0, buffer[idx]);
			
			//short delay
			inb(0x323);
			inb(0x323);
			inb(0x323);
		}
		buffer += idx;
	}
	outb(0x1F7, 0xE7); //send cache clear
	while (inb(0x1F7) & 0x80) //wait for drive to finish clearing cache
	{
	}
   return 0; 
}

// Read sector(s) from booted partition to 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_read( unsigned long sector_offset, unsigned char num_blocks ) {
   return int13h_read_o(sector_offset, num_blocks, 0); 
}

// Write sector(s) to booted partition from 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_write( unsigned long sector_offset, unsigned char num_blocks ) {
	return int13h_write_o (sector_offset, num_blocks, 0);
}