#ifndef UTILS_H
#define UTILS_H

long get_file_size(const char *filename);
int is_text_file(const char *filename);
int get_file_permissions(const char *filename, char *perm_str);

#endif
