#include "../include/terminal.h"

/* compare two null-terminated strings — returns 1 if equal, 0 if not */
static int streq(const char* a, const char* b)
{
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

/*
 * Process a command string entered by the user.
 * Called from kernel.c when the user presses Enter.
 */
void shell_run(char* input)
{
    if (streq(input, "help")) {
        terminal_write("Commands:\n");
        terminal_write("  help    - show this list\n");
        terminal_write("  clear   - clear the screen\n");
        terminal_write("  hello   - print a greeting\n");
    }
    else if (streq(input, "clear")) {
        terminal_init();   /* wipe the screen and reset cursor */
    }
    else if (streq(input, "hello")) {
        terminal_write("Hello, world!\n");
    }
    else if (input[0] == 0) {
        /* empty input — do nothing */
    }
    else {
        terminal_write("Unknown command: ");
        terminal_write(input);
        terminal_write("\n");
    }

    /* print the prompt for the next command */
    terminal_write("MyOS> ");
}