#ifndef FS_H
#define FS_H

int fs_create(const char *name);
int fs_write(const char *name, const char *data);
const char *fs_read(const char *name);
void fs_list();
int fs_delete(const char *name);

int fs_mkdir(const char *name);
void fs_pwd();
int fs_cd(const char *name);
const char *fs_get_path();

#endif