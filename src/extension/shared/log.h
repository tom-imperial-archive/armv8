/*
Shared debugging utility, with logging macros
*/

#include <stdio.h>
#include <stdbool.h>

extern FILE *log_out;
extern FILE *log_err;

extern bool init_logger(const char *filepath);
extern void close_logger(void);

// Macros
#define LOG_INFO(fmt, ...) fprintf(log_out, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) fprintf(log_out, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(log_err, "[ERROR] " fmt "\n", ##__VA_ARGS__)
