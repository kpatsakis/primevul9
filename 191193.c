read_descr_size (guint8 * ptr, guint8 * end, guint8 ** end_out)
{
  int count = 4;
  int len = 0;

  while (count--) {
    int c;

    if (ptr >= end)
      return -1;

    c = *ptr++;
    len = (len << 7) | (c & 0x7f);
    if (!(c & 0x80))
      break;
  }
  *end_out = ptr;
  return len;
}