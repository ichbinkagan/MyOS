#include "../include/fs.h"
#include "../include/terminal.h"

// define
#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256
#define MAX_DIRS 16
#define MAX_PATH 128

// create structs
struct File
{
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    int used;
};

struct Dir
{
    char name[MAX_FILENAME];
    int used;
};

static struct File files[MAX_FILES];
static struct Dir dirs[MAX_DIRS];

// current path
static char current_path[MAX_PATH] = "/";

/* ---- Helpful Functions ---- */

static void str_copy(char *dest, const char *src, int max)
{
    int i = 0;
    while (src[i] && i < max - 1)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

static int str_len(const char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return i;
}

static int str_eq(const char *a, const char *b)
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

/* ---- File Functions ---- */

// creating files(mkdir)
int fs_create(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used == 0)
        {
            str_copy(files[i].name, name, MAX_FILENAME);
            files[i].data[0] = 0;
            files[i].used = 1;
            return 1;
        }
    }
    return 0;
}

// write files
int fs_write(const char *name, const char *data)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
        {
            str_copy(files[i].data, data, MAX_FILESIZE);
            return 1;
        }
    }
    return 0;
}

// read files
const char *fs_read(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
            return files[i].data;
    }
    return 0;
}

// list files
void fs_list()
{
    // found files
    int found = 0;

    // show dir
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used)
        {
            terminal_write("[dir] ");
            terminal_write(dirs[i].name);
            terminal_write("\n");
            found = 1;
        }
    }

    // show files
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used)
        {
            terminal_write("[file] ");
            terminal_write(files[i].name);
            terminal_write("\n");
            found = 1;
        }
    }

    // if there is no file, print "empty"
    if (!found)
        terminal_write("(empty)\n");
}

// delete files
int fs_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
        {
            files[i].used = 0;
            return 1;
        }
    }
    return 0;
}

/* ---- Dir Functions ---- */

int fs_mkdir(const char *name)
{
    /* check if already exists */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used && str_eq(dirs[i].name, name))
        {
            terminal_write("Error: directory already exists\n");
            return 0; /* süslü parantez içine al */
        }
    }

    /* find empty slot */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used == 0)
        {
            str_copy(dirs[i].name, name, MAX_FILENAME);
            dirs[i].used = 1;
            return 1;
        }
    }

    terminal_write("Error: directory table full\n");
    return 0;
}

// pwd(show directory)
void fs_pwd()
{
    terminal_write(current_path);
    terminal_write("\n");
}

int fs_cd(const char *name)
{
    if (str_eq(name, "/"))
    {
        current_path[0] = '/'; /* " değil ' */
        current_path[1] = 0;
        return 1;
    }

    if (str_eq(name, ".."))
    {
        if (str_eq(current_path, "/"))
            return 1;

        int len = str_len(current_path);
        for (int i = len - 1; i > 0; i--)
        {
            if (current_path[i] == '/') /* " değil ' */
            {
                current_path[i] = 0;
                break;
            }
        }
        return 1;
    }

    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used && str_eq(dirs[i].name, name))
        {
            if (!str_eq(current_path, "/"))
            {
                int len = str_len(current_path);
                current_path[len] = '/';
                str_copy(current_path + len + 1, name, MAX_PATH - len - 1);
            }
            else
            {
                current_path[0] = '/';
                str_copy(current_path + 1, name, MAX_PATH - 1);
            }
            return 1;
        }
    }

    terminal_write("Error: directory not found\n");
    return 0;
}

const char *fs_get_path()
{
    return current_path;
}