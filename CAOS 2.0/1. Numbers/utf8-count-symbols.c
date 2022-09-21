#include <stdio.h>

int main()
{
  char buffer[10];
  int max_len = (int)sizeof(buffer) - 1, actual_len = 0;
  char format[32]; // Гарантированно достаточный буффер для генерируемой форматной строки.
  snprintf(format, sizeof(format), "%%%ds%%n", (int)max_len);
  if (sscanf("aboba\nk", format, buffer, &actual_len) == 1 && actual_len != max_len) {
    printf("complete read: %s\n", buffer);
  } else {
    printf("incomplete read: %s\n", buffer);
  }
}
