#include "../include/terminal.h"
#include "../include/fs.h"

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256

/* compare two null-terminated strings — returns 1 if equal, 0 if not */
static int streq(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

/*
 * Process a command string entered by the user.
 * Called from kernel.c when the user presses Enter.
 */
void shell_run(char *input)
{
    if (streq(input, "help"))
    {
        terminal_write("Commands:\n");
        terminal_write("  help    - show this list\n");
        terminal_write("  clear   - clear the screen\n");
        terminal_write("  hello   - print a greeting\n");
        terminal_write("  pwd   - print the directory\n");
        terminal_write("  mkdir   - create a file\n");
        terminal_write("  cd     - go to a directory\n");
    }
    else if (streq(input, "clear"))
    {
        terminal_init(); /* wipe the screen and reset cursor */
    }
    else if (streq(input, "hello"))
    {
        terminal_write("Hello, world!\n");
    }
    else if (streq(input, "pwd"))
    {
        fs_pwd();
    }
    else if (input[0] == 'm' && input[1] == 'k' && input[2] == 'd' && input[3] == 'i' && input[4] == 'r' && input[5] == ' ')
    {
        const char *name = input + 6;
        if (name[0] == 0)
        {
            terminal_write("Usage: mkdir <name>\n");
        }
        else if (fs_mkdir(name))
        {
            terminal_write("Created: ");
            terminal_write(name);
            terminal_write("\n");
        }
    }
    else if (input[0] == 'c' && input[1] == 'd' && input[2] == ' ')
    {
        fs_cd(input + 3);
    }
    else if (streq(input, "ls"))
    {
        fs_list();
    }
    else if (input[0] == 0)
    {
        /* empty input — do nothing */
    }
    else
    {
        terminal_write("Unknown command: ");
        terminal_write(input);
        terminal_write("\n");
    }

    /* print the prompt for the next command */
    terminal_write("MyOS:");
    terminal_write(fs_get_path());
    terminal_write("> ");
}