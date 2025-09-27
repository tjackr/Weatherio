#ifndef JANSSON_CONFIG_H
#define JANSSON_CONFIG_H

/* Ensure we have standard integer types */
#include <stdint.h>
#include <inttypes.h>

/* Basic headers */
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_SYS_TYPES_H 1

/* JSON_INTEGER format */
#define JSON_INTEGER_FORMAT "lld"
#define JSON_INTEGER_IS_LONG_LONG 1

/* Inline - just inline, not static inline (jansson.h already has static) */
#define JSON_INLINE inline

/* Locale support */
#define HAVE_LOCALE_H 1
#define HAVE_SETLOCALE 1

#ifndef JSON_PARSER_MAX_DEPTH
#define JSON_PARSER_MAX_DEPTH 2048
#endif

#endif /* JANSSON_CONFIG_H */