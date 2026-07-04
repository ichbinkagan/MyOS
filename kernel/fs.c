#include "../include/fs.h"
#include "../include/terminal.h"

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILESIZE 256
#define MAX_DIRS 16
#define MAX_PATH 128

/* file struct — holds name, content and a flag showing if the slot is used */
struct File
{
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    int used;
};

/* directory struct — holds name and a flag showing if the slot is used */
struct Dir
{
    char name[MAX_FILENAME];
    int used;
};

static struct File files[MAX_FILES];
static struct Dir dirs[MAX_DIRS];

/* current working directory path — starts at root */
static char current_path[MAX_PATH] = "/";

/* ---- helper functions ---- */

/* copy src into dest, stop at max-1 characters, always null-terminate */
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

/* return the length of a null-terminated string */
static int str_len(const char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return i;
}

/* return 1 if two strings are equal, 0 if not */
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

/* ---- file functions ---- */

/* create a new file — find an empty slot, store the name, mark as used */
int fs_create(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used == 0)
        {
            str_copy(files[i].name, name, MAX_FILENAME);
            files[i].data[0] = 0;
            files[i].used = 1;
            return 1; /* success */
        }
    }
    return 0; /* no empty slot */
}

/* write data into an existing file */
int fs_write(const char *name, const char *data)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
        {
            str_copy(files[i].data, data, MAX_FILESIZE);
            return 1; /* success */
        }
    }
    return 0; /* file not found */
}

/* return a pointer to the file contents, or 0 if not found */
const char *fs_read(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
            return files[i].data;
    }
    return 0;
}

/* list all directories and files in the current filesystem */
void fs_list()
{
    int found = 0;

    /* print directories first */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used)
        {
            terminal_write("[dir]  ");
            terminal_write(dirs[i].name);
            terminal_write("\n");
            found = 1;
        }
    }

    /* then print files */
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

    if (!found)
        terminal_write("(empty)\n");
}

/* delete a file by marking its slot as unused */
int fs_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
        {
            files[i].used = 0;
            return 1; /* success */
        }
    }
    return 0; /* file not found */
}

/* ---- directory functions ---- */

/* create a new directory — check for duplicates, then find an empty slot */
int fs_mkdir(const char *name)
{
    /* check if a directory with this name already exists */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used && str_eq(dirs[i].name, name))
        {
            terminal_write("Error: already exists\n");
            return 0;
        }
    }

    /* find an empty slot and store the name */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used == 0)
        {
            str_copy(dirs[i].name, name, MAX_FILENAME);
            dirs[i].used = 1;
            return 1; /* success */
        }
    }

    terminal_write("Error: directory table full\n");
    return 0;
}

/* print the current working directory path */
void fs_pwd()
{
    terminal_write(current_path);
    terminal_write("\n");
}

/* change the current directory */
int fs_cd(const char *name)
{
    /* go to root */
    if (name[0] == '/' && name[1] == 0)
    {
        current_path[0] = '/';
        current_path[1] = 0;
        return 1;
    }

    /* go up one directory */
    if (name[0] == '.' && name[1] == '.' && name[2] == 0)
    {

        /* already at root — do nothing */
        if (current_path[0] == '/' && current_path[1] == 0)
            return 1;

        /* find the last '/' and cut the string there */
        int len = str_len(current_path);
        for (int i = len - 1; i >= 0; i--)
        {
            if (current_path[i] == '/')
            {
                if (i == 0)
                {
                    /* parent is root — keep the leading slash */
                    current_path[1] = 0;
                }
                else
                {
                    current_path[i] = 0;
                }
                break;
            }
        }
        return 1;
    }

    /* look for the directory in the directory table */
    for (int i = 0; i < MAX_DIRS; i++)
    {
        if (dirs[i].used && str_eq(dirs[i].name, name))
        {

            int len = str_len(current_path);

            /* add a slash if we are not already at root */
            if (current_path[len - 1] != '/')
            {
                current_path[len] = '/';
                len++;
            }

            /* append the directory name to the path */
            str_copy(current_path + len, name, MAX_PATH - len);
            return 1;
        }
    }

    terminal_write("Error: directory not found\n");
    return 0;
}

/* return the current working directory path */
const char *fs_get_path()
{
    return current_path;
}

int fs_rm(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_eq(files[i].name, name))
        {
            files[i].used = 0;
            files[i].name[0] = 0;
            files[i].data[0] = 0;
            return 1;
        }
    }

    return 0;
}