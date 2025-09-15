#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct {
    int temp; /* use 0/1 as boolean */
} http;

int  http_init(http *h);
void http_dispose(http *h);

int  http_get(const char *url, char *buf, size_t bufsize);

#endif /* HTTP_H */
