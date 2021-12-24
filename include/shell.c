#include "shell.h"
#include <"../cpu/detect.h">
#include "../debug/printf.h"
#include "../drivers/acpi.h"
#include "../drivers/reboot.h"
#include "../drivers/rtc.h"
#include ../drivers/screen.h
#include panic.h
#include <stdlib.h>
#include string.h
#include <stdbool.h>
#include common.h
#include terminal.h
#include keyboard.h
#include stdfunc.h
#include filesystem.h
#include <stdio.h>
#include syscalls.h

int dis_print = 0;

void print_prompt() {
  printf("> ");
}

void handle_ls(char* input) {
  bool flag_long = false;
  char* flag = input + 2;
  while (*flag == ' ') {
    flag++;
  }
  if (*flag != 0) {
    if (strncmp(flag, "-l", 2) == 0) {
      flag_long = true;
    } else {
      printf("Unrecognized flag `%s`\n", flag);
      return;
    }
  }
  struct file_t files[10];
  uint32_t num_files = count_files();
  list_files(files);
  for (uint32_t i = 0; i < num_files; i++) {
    if (flag_long) {
      printf("%s (%d bytes)\n", files[i].name, files[i].size);
    } else {
      printf("%s\n", files[i].name);
    }
  }
}

void handle_input(char* input) {
  if (strncmp(input, "ls", 2) == 0) {
    handle_ls(input);
  } else {
    printf("Unknown command `%s`\n", input);
  }
  printf("> ");
}

int main() {
  print_prompt();
  register_input_handler(handle_input);
  while(1){}

  return 0;
}

void shell(const char *input) {
    if (strcmp(input, "end") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        halt();
    } else if (strcmp(input, "page") == 0) {
        printf("Page: 0x%lX\n",
               (uint32_t)aligned_alloc(1000, sizeof(uint32_t)));
    } else if (strcmp(input, "clear") == 0) {
        clear_screen(bgcolor);
        dis_print = 1;
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strcmp(input, "shutdown") == 0) {
        acpi_shutdown();
    } else if (strcmp(input, "cpuinfo") == 0) {
        cpudetect();
    } else if (strcmp(input, "rtc") == 0) {
        rtctime();
    } else if (strncmp(input, "echo ", 4) == 0) {
        size_t i;
        char *sp = (char *)malloc(sizeof(char) * 256);
        for (i = 5; i < strlen(input); ++i) {
            append(sp, input[i]);
        }

        if (strcmp(sp, "\\?") == 0) {
            extern int globstatus;
            printf("%i", globstatus);
        } else {
            kprint(sp);
        }

        kprint("\n");
    } else if (strcmp(input, "help") == 0) {
        kprint("Welcome to PawsOS, these are all the commands:\n"
               "\"end\": Halt the CPU.\n"
               "\"page\": Request a malloc().\n"
               "\"clear\": Clear the screen.\n"
               "\"cpuinfo\": Get information about your CPU.\n"
               "\"rtc\" Get the current time (in UTC).\n"
               "\"reboot\": Reboot your computer.\n"
               "\"echo [argument]\": Print something on the screen.\n"
               "\"shutdown\": Shutdown your computer.\n"
               "\"ver\": Show current version.\n"
               "Press Left shift to enable AZERTY keyboard layout.\n"
               "Press the Up key to print last used command.\n");
    } else if (strcmp(input, "ver") == 0) {
        kprint("PawsOS Version 0.04 (unreleased)\n");
    }

    if (!dis_print) {
        printf("You said: %s\n> ", input);
    } else if (dis_print == 1) {
        kprint("> ");
        dis_print = 0;
    } else if (dis_print == 2) {
        dis_print = 0;
    } else {
        PANIC(__FILE__, __LINE__, "No valid value in \"dis_print\" variable.");
    }
}

char shell_buffer[SHELL_BUFFER_SIZE];
uint16_t shell_buffersize = 0;
bool running = false;

void shell_bufferclear()
{
    shell_buffersize = 0;
}

void shell_bufferpush(char c)
{
    if(shell_buffersize < SHELL_BUFFER_SIZE)
    {
        shell_buffer[shell_buffersize] = c;
        shell_buffersize++;
    }
}

char shell_bufferpop()
{
    if(shell_buffersize)
    {
        shell_buffersize--;
        return shell_buffer[shell_buffersize];
    }

    return 0;
}

char* shell_bufferread()
{
    shell_bufferpush('\0');
    return shell_buffer;
}

void shell_clear()
{
    terminal_writestring("mato:myos/$ ");
    shell_bufferclear();
}

void shell_exit()
{
    shell_bufferclear();
    terminal_clearscreen();
}

void shell_command_help(int argc, char *argv[])
{
    terminal_writestring("Syntax: help <prikaz>\n");
    terminal_writestring("clear -> Vycisti obrazovku\n");
}

void shell_command_clear()
{
    terminal_clearscreen();
    shell_bufferclear();
}

void shell_command_color(int argc, char *argv[])
{
    if (argc == 2) {
        uint8_t fg = atoi(argv[1]);
        uint8_t bg = atoi(argv[2]);
        terminal_setcolor(make_color_s(fg, bg));
    } else {
        terminal_writestring("Zadal si nepravny pocet argumentov: ");
        terminal_writestring(itoa(argc, 10));
        terminal_putchar('\n');
    }
}

void shell_command_test(int argc, char *argv[])
{
    terminal_writestring("Test message!\n");
    terminal_writestring(argv[0]);
    terminal_putchar('\n');
}

void shell_command_exit()
{
    running = false;
    terminal_writestring("[+]Exiting...");
}

void shell_interpreter(char *command)
{
    int argc = 0;
    char argv[32][32];
    int start = 0;
    int current = 0;
    int i;
    int j = 0;

    while(command[current] != '\0')
    {
        if(command[current] == ' ' || command[current] == '\n')
        {
            j = 0;
            for(i = start; i < current; i++)
            {
                argv[argc][j] = command[i];
                j++;
            }

            argv[argc][j] = '\0';
            argc++;

            start = current + 1;

        }

        current++;
    }

    argc--;

    if(strcmp(argv[0], "test") == 0)
    {
        shell_command_test(argc, argv);
    }
    else if (strcmp(argv[0], "help") == 0)
    {
        shell_command_help(argc, argv);
    }
    else if (strcmp(argv[0], "color") == 0)
    {
        shell_command_color(argc, argv);
    }
    else if (strcmp(argv[0], "clear") == 0)
    {
        shell_command_clear();
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        shell_command_exit();
    }
    else if (strcmp(argv[0], "") == 0)
    {
        //Do nothing
    }
    else
    {
        terminal_writestring(argv[0]);
        terminal_writestring(": Prikaz nebol najdeny\n");
    }

    shell_clear();
}

void InitializeShell()
{
    terminal_writestring("MatoOS\n");
    terminal_writestring("Pre pomoc pouzi prikaz help\n");

    shell_clear();

    running = true;

    while(running)
    {
        char c;

        if((c = getchar()))
        {
            if(c == '\b')
            {
                if(shell_bufferpop())
                {
                    terminal_putchar('\b');
                }
            }
            else 
            {
                shell_bufferpush(c);
                terminal_putchar(c);
            }

            if(c == '\n')
            {
                shell_interpreter(shell_bufferread());
            }    
        }
    }

    shell_exit();
}