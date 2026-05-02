# strview_c

Small single-header string view library for C.

`strview_t` is a non-owning view into existing string data. The library does not
allocate memory and does not modify the underlying bytes.

## Features

- Non-owning `strview_t { const char *data; size_t length; }`
- Single-header library with implementation block
- No dynamic allocation
- Literal macros for expression and static-initializer contexts
- Basic compare, search, slicing, trimming, copying, and splitting

## Integration

In one translation unit:

```c
#define STRVIEW_IMPLEMENTATION
#include "strview/strview.h"
```

In other files:

```c
#include "strview/strview.h"
```

## Example

```c
#define STRVIEW_IMPLEMENTATION
#include "strview/strview.h"

#include <stdio.h>

#define s STRVIEW_LIT

int main(void) {
    strview_t text = s("one,two,three");
    strview_t part = {0};

    while (strview_split(text, s(","), &part)) {
        printf("%.*s\n", (int)part.length, part.data);
    }

    return 0;
}
```

For static initializers, use `STRVIEW_INIT`:

```c
static const strview_t name = STRVIEW_INIT("name");
```

## Split Contract

`strview_split()` uses `part` as iteration state.

- Initialize `part` to `{0}` before the first call.
- Reuse the same `part` variable for subsequent calls.
- Empty separators are rejected and return `false`.

## API

- `strview_from_parts`
- `strview_from_cstr`
- `STRVIEW_INIT`
- `STRVIEW_LIT`
- `strview_is_empty`
- `strview_compare`
- `strview_equals`
- `strview_starts_with`
- `strview_ends_with`
- `strview_find`
- `strview_find_char`
- `strview_contains`
- `strview_slice`
- `strview_trim_left`
- `strview_trim_right`
- `strview_trim`
- `strview_copy`
- `strview_split`
