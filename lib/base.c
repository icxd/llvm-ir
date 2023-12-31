#include <lib/base.h>

void arena_init(arena_t *a) {
    a->ptr = NULL;
    a->size = 0;
    a->capacity = 0;
}

void arena_free(arena_t *a) {
    free(a->ptr);
    a->ptr = NULL;
    a->size = 0;
    a->capacity = 0;
}

void *arena_alloc(arena_t *a, size_t size) {
    if (a->size + size > a->capacity) {
        size_t new_capacity = a->capacity * 2;
        if (new_capacity < 16) {
            new_capacity = 16;
        }
        if (new_capacity < a->size + size) {
            new_capacity = a->size + size;
        }

        char *new_ptr = (char *)malloc(new_capacity);
        if (!new_ptr) {
            fprintf(stderr, "failed to allocate memory for new_ptr");
            return NULL;
        }

        memcpy(new_ptr, a->ptr, a->size);
        free(a->ptr);
        a->ptr = new_ptr;
        a->capacity = new_capacity;
    }

    void *result = a->ptr + a->size;
    a->size += size;
    return result;
}

void arena_clear(arena_t *a) {
    a->size = 0;
}


char str_at(str *s, size_t idx) {
    assert(idx < s->count);
    return s->chars[idx];
}

bool str_eq(str s1, str s2) {
    if (s1.count != s2.count) {
        return false;
    }

    for (size_t i = 0; i < s1.count; i++) {
        if (s1.chars[i] != s2.chars[i]) {
            return false;
        }
    }

    return true;
}

void str_append(str *s1, str s2) {
    size_t new_length = s1->count + s2.count;

    char *new_chars = (char *)malloc((new_length + 1) * sizeof(char));
    if (!new_chars) {
        fprintf(stderr, "failed to allocate memory for new_chars");
        return;
    }

    memcpy(new_chars, s1->chars, s1->count * sizeof(char));
    memcpy(new_chars + s1->count, s2.chars, s2.count * sizeof(char));
    new_chars[new_length] = '\0';

    s1->chars = new_chars;
    s1->count = new_length;
}

void str_append_cstr(str *s1, char *s2) {
    str_append(s1, STR(s2));
}

void str_append_int(str *s1, int i) {
    char buf[32];
    sprintf_s(buf, 32, "%d", i);
    str_append_cstr(s1, buf);
}

void str_append_float(str *s1, float f) {
    char buf[32];
    sprintf_s(buf, 32, "%f", f);
    str_append_cstr(s1, buf);
}

void str_append_double(str *s1, double d) {
    char buf[32];
    sprintf_s(buf, 32, "%f", d);
    str_append_cstr(s1, buf);
}

void str_append_char(str *s1, char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    str_append_cstr(s1, buf);
}

str str_substring(str s, size_t start, size_t end) {
    if (start > s.count || end > s.count)
        return (str){NULL, 0};
    return (str){s.chars + start, end - start};
}

void str_free(str *s) {
    free(s->chars);
    s->chars = NULL;
    s->count = 0;
}

str file_read_to_str(str path) {
    FILE* filepoint;
    errno_t err;

    if ((err = fopen_s(&filepoint, path.chars, "rb")) != 0) {
        fatal("Failed to open file '" STR_ARG "' for reading.\n", STR_FMT(path));
    } else {
        fseek(filepoint, 0, SEEK_END);
        long size = ftell(filepoint);
        fseek(filepoint, 0, SEEK_SET);

        char* raw_contents = malloc(size + 1);
        fread(raw_contents, 1, size, filepoint);
        raw_contents[size] = '\0';

        fclose(filepoint);
        return STR(raw_contents);
    }
}

void file_write(str path, str contents) {
    FILE* filepoint;
    errno_t err;

    if ((err = fopen_s(&filepoint, path.chars, "wb")) != 0) {
        fatal("Failed to open file '" STR_ARG "' for writing.\n", STR_FMT(path));
    } else {
        fwrite(contents.chars, 1, contents.count, filepoint);
        fclose(filepoint);
    }
}
