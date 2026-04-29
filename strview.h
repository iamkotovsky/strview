/*
 * strview.h
 * Version: 0.1.1
 * License: MIT
 * Repository: https://github.com/iamkotovsky/strview
 * Description: Small single-header string view library for C.
 *
 * Define STRVIEW_IMPLEMENTATION in exactly one translation unit before
 * including this header to emit the function definitions.
 */
#ifndef STRVIEW_H
#define STRVIEW_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct strview {
    const char *data;
    size_t length;
} strview_t;

#define STRVIEW_NPOS ((size_t)-1)
#define STRVIEW_LIT(cstr)                                                      \
    strview_from_parts((cstr), (sizeof(cstr) / sizeof((cstr)[0])) - 1U)

static inline strview_t strview_from_parts(const char *data, size_t length) {
    strview_t view;
    view.data = data;
    view.length = length;
    return view;
}

static inline strview_t strview_from_cstr(const char *cstr) {
    if (cstr == NULL) {
        return strview_from_parts(NULL, 0);
    }

    return strview_from_parts(cstr, strlen(cstr));
}

static inline bool strview_is_empty(strview_t str) { return str.length == 0U; }

static inline bool strview_is_space(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' ||
           ch == '\v';
}

/* Lexicographically compare two views. */
int strview_compare(strview_t left, strview_t right);

/* Return true when both views contain the same bytes. */
bool strview_equals(strview_t left, strview_t right);

/* Return true when str begins with prefix. */
bool strview_starts_with(strview_t str, strview_t prefix);

/* Return true when str ends with suffix. */
bool strview_ends_with(strview_t str, strview_t suffix);

/* Find needle in str and return its first index or STRVIEW_NPOS. */
size_t strview_find(strview_t str, strview_t needle);

/* Find ch in str and return its first index or STRVIEW_NPOS. */
size_t strview_find_char(strview_t str, char ch);

/* Return true when needle appears anywhere in str. */
bool strview_contains(strview_t str, strview_t needle);

/* Return a subview or {NULL, 0} when the requested range is out of bounds. */
strview_t strview_slice(strview_t str, size_t start, size_t length);

/* Trim ASCII whitespace from the left side of str. */
strview_t strview_trim_left(strview_t str);

/* Trim ASCII whitespace from the right side of str. */
strview_t strview_trim_right(strview_t str);

/* Trim ASCII whitespace from both ends of str. */
strview_t strview_trim(strview_t str);

/* Copy into dest. */
size_t strview_copy(char *dest, size_t dest_size, strview_t src);

/* Copy into dest and NUL-terminate when dest_size is non-zero. */
size_t strview_to_cstr(char *dest, size_t dest_size, strview_t src);

/*
 * Iterate str split by separator using part as iteration state.
 * Initialize part to {0} before the first call.
 */
bool strview_split(strview_t str, strview_t separator, strview_t *part);

#ifdef STRVIEW_IMPLEMENTATION

int strview_compare(strview_t left, strview_t right) {
    size_t common_length =
        left.length < right.length ? left.length : right.length;
    int cmp;

    if (common_length > 0U) {
        cmp = memcmp(left.data, right.data, common_length);
        if (cmp != 0) {
            return cmp;
        }
    }

    if (left.length < right.length) {
        return -1;
    }

    if (left.length > right.length) {
        return 1;
    }

    return 0;
}

bool strview_equals(strview_t left, strview_t right) {
    return strview_compare(left, right) == 0;
}

bool strview_starts_with(strview_t str, strview_t prefix) {
    if (prefix.length > str.length) {
        return false;
    }

    if (prefix.length == 0U) {
        return true;
    }

    return memcmp(str.data, prefix.data, prefix.length) == 0;
}

bool strview_ends_with(strview_t str, strview_t suffix) {
    if (suffix.length > str.length) {
        return false;
    }

    if (suffix.length == 0U) {
        return true;
    }

    return memcmp(str.data + (str.length - suffix.length), suffix.data,
                  suffix.length) == 0;
}

size_t strview_find(strview_t str, strview_t needle) {
    size_t limit;

    if (needle.length == 0U) {
        return 0U;
    }

    if (needle.length > str.length) {
        return STRVIEW_NPOS;
    }

    limit = str.length - needle.length;
    for (size_t i = 0U; i <= limit; ++i) {
        if (memcmp(str.data + i, needle.data, needle.length) == 0) {
            return i;
        }
    }

    return STRVIEW_NPOS;
}

size_t strview_find_char(strview_t str, char ch) {
    for (size_t i = 0U; i < str.length; ++i) {
        if (str.data[i] == ch) {
            return i;
        }
    }

    return STRVIEW_NPOS;
}

bool strview_contains(strview_t str, strview_t needle) {
    return strview_find(str, needle) != STRVIEW_NPOS;
}

strview_t strview_slice(strview_t str, size_t start, size_t length) {
    if (start > str.length) {
        return strview_from_parts(NULL, 0U);
    }

    if (length > str.length - start) {
        return strview_from_parts(NULL, 0U);
    }

    return strview_from_parts(str.data + start, length);
}

strview_t strview_trim_left(strview_t str) {
    size_t start = 0U;

    while (start < str.length && strview_is_space(str.data[start])) {
        ++start;
    }

    return strview_from_parts(str.data + start, str.length - start);
}

strview_t strview_trim_right(strview_t str) {
    size_t end = str.length;

    while (end > 0U && strview_is_space(str.data[end - 1U])) {
        --end;
    }

    return strview_from_parts(str.data, end);
}

strview_t strview_trim(strview_t str) {
    return strview_trim_right(strview_trim_left(str));
}

size_t strview_copy(char *dest, size_t dest_size, strview_t src) {
    if (dest == NULL || dest_size == 0 || src.length == 0) {
        return 0;
    }

    size_t copy_length = src.length < dest_size ? src.length : dest_size;
    memcpy(dest, src.data, copy_length);

    return copy_length;
}

size_t strview_to_cstr(char *dest, size_t dest_size, strview_t src) {
    if (dest == NULL || dest_size == 0) {
        return 0;
    }

    size_t copy_length = src.length < dest_size ? src.length : dest_size - 1;
    strview_copy(dest, dest_size - 1, src);
    dest[copy_length] = 0;

    return copy_length;
}

bool strview_split(strview_t str, strview_t separator, strview_t *part) {
    const char *start;
    const char *end;

    if (part == NULL || separator.length == 0U) {
        return false;
    }

    start = (part->data == NULL) ? str.data
                                 : part->data + part->length + separator.length;
    end = str.data + str.length;

    if (start > end) {
        return false;
    }

    for (const char *current = start; current + separator.length <= end;
         ++current) {
        if (memcmp(current, separator.data, separator.length) == 0) {
            *part = strview_from_parts(start, (size_t)(current - start));
            return true;
        }
    }

    *part = strview_from_parts(start, (size_t)(end - start));
    return true;
}

#endif

#endif
