/* http.c */
#include "http.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

/* simple sink that appends into a bounded buffer with truncation */
struct sink {
    char  *buf;
    size_t cap;
    size_t len;
};

static size_t write_to_buf(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    struct sink *s;
    size_t bytes;
    size_t free_space;
    size_t to_copy;

    s = (struct sink*)userdata;
    bytes = size * nmemb;

    if (s->cap == 0 || s->len >= s->cap) return bytes;

    free_space = (s->cap > s->len) ? (s->cap - s->len - 1) : 0;
    to_copy = (bytes < free_space) ? bytes : free_space;

    if (to_copy > 0) {
        memcpy(s->buf + s->len, ptr, to_copy);
        s->len += to_copy;
        s->buf[s->len] = '\0';
    }
    /* report all bytes consumed even if truncated */
    return bytes;
}

int http_init(http *h)
{
    CURLcode g;
    if (!h) return -1;
    h->temp = 1;

    g = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (g != CURLE_OK) {
        fprintf(stderr, "curl_global_init failed: %s\n", curl_easy_strerror(g));
        return -2;
    }
    return 0;
}

void http_dispose(http *h)
{
    if (h) h->temp = 0;
    curl_global_cleanup();
}

int http_get(const char *url, char *buf, size_t bufsize)
{
    CURL *curl;
    CURLcode rc;
    struct sink s;

    if (!url || !buf || bufsize == 0) return -10;

    buf[0] = '\0';
    s.buf = buf;
    s.cap = bufsize;
    s.len = 0;

    curl = curl_easy_init();
    if (!curl) return -11;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buf);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&s);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    rc = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (rc != CURLE_OK) {
        fprintf(stderr, "CURL failed: %s\n", curl_easy_strerror(rc));
        return -12;
    }
    return 0;
}
