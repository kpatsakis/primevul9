gst_matroska_ebmlnum_uint (guint8 * data, guint size, guint64 * num)
{
  gint len_mask = 0x80, read = 1, n = 1, num_ffs = 0;
  guint64 total;

  if (size <= 0) {
    return -1;
  }

  total = data[0];
  while (read <= 8 && !(total & len_mask)) {
    read++;
    len_mask >>= 1;
  }
  if (read > 8)
    return -1;

  if ((total &= (len_mask - 1)) == len_mask - 1)
    num_ffs++;
  if (size < read)
    return -1;
  while (n < read) {
    if (data[n] == 0xff)
      num_ffs++;
    total = (total << 8) | data[n];
    n++;
  }

  if (read == num_ffs && total != 0)
    *num = G_MAXUINT64;
  else
    *num = total;

  return read;
}