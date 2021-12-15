#include "shell.h"
#include "../cpu/detect.h"
#include "../debug/printf.h"
#include "../drivers/acpi.h"
#include "../drivers/reboot.h"
#include "../drivers/rtc.h"
#include "../drivers/screen.h"
#include "panic.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "common.h"
#include "terminal.h"
#include "keyboard.h"
#include "stdfunc.h"

int dis_print = 0;

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