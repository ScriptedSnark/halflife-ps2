#ifndef COMMON_H
#define COMMON_H

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef size_t
typedef unsigned int size_t;
#endif

//============================================================================


void Q_memset(void* dest, int fill, size_t count);
void Q_memcpy(void* dest, void* src, size_t count);
int Q_memcmp(void* m1, void* m2, size_t count);
void Q_strcpy(char* dest, char* src);
void Q_strncpy(char* dest, char* src, size_t count);
int Q_strlen(char* str);
char* Q_strrchr(char* s, char c);
void Q_strcat(char* dest, char* src);
int Q_strcmp(char* s1, char* s2);
int Q_strncmp(char* s1, char* s2, size_t count);
int Q_strcasecmp(char* s1, char* s2);
int Q_strncasecmp(char* s1, char* s2, int n);
int	Q_atoi(char* str);
float Q_atof(char* str);

int Sys_rand();
void Sys_sprintf(char* buffer, const char* format, ...);
void* Q_malloc(int size);
void Q_free(void* ptr);

//============================================================================

#endif