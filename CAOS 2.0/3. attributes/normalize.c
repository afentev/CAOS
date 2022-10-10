void reverse(char* path, int length)
{
    for (int i = 0; i * 2 < length; ++i) {
        char tmp = *(path + i);
        *(path + i) = *(path + length - i - 1);
        *(path + length - i - 1) = tmp;
    }
}

void cyclic_shift(char* path, int length, int shift)
{
    if (shift < 0) {
        shift += length;
    }
    reverse(path, length);
    reverse(path, shift);
    reverse(path + shift, length - shift);
}

void normalize_path(char* path)
{
    int size = 0;
    while (*(path + size) != '\0') {
        ++size;
    }
    int pos = 0;
    char prev, chr;
    while ((chr = *(path + pos)) != '\0') {
        if (pos != 0) {
            prev = *(path + pos - 1);
        } else {
            prev = '\0';
        }
        char next = *(path + pos + 1);
        if (prev == '/' && chr == '/') {
            cyclic_shift(path + pos, size - pos + 1, -1);
            --pos;
        } else if (prev == '/' && chr == '.' && (next == '/' || next == '\0')) {
            cyclic_shift(path + pos - 1, size - pos + 2, -2);
            pos -= 2;
        } else if (
            prev == '/' && chr == '.' && next == '.' &&
            (*(path + pos + 2) == '/' || *(path + pos + 2) == '\0')) {
            int left_border = pos - 2;
            while (left_border >= 0 && *(path + left_border) != '/') {
                --left_border;
            }
            if (left_border < 0) {
                // error has occurred
                return;
            }
            cyclic_shift(
                path + left_border, size - left_border, left_border - pos - 2);
            pos = left_border - 1;
        }
        ++pos;
    }
}