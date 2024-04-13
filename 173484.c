convert_1_bit (const gchar *src,
               gchar       *dst,
               guint32      rows,
               guint32      columns)
{
/* Convert bits to bytes left to right by row.
   Rows are padded out to a byte boundry.
*/
  guint32 row_pos = 0;
  gint    i, j;

  IFDBG(3)  g_debug ("Start 1 bit conversion");

  for (i = 0; i < rows * ((columns + 7) >> 3); ++i)
    {
      guchar    mask = 0x80;
      for (j = 0; j < 8 && row_pos < columns; ++j)
        {
          *dst = (*src & mask) ? 0 : 1;
          IFDBG(3) g_debug ("byte %d, bit %d, offset %d, src %d, dst %d",
            i , j, row_pos, *src, *dst);
          dst++;
          mask >>= 1;
          row_pos++;
        }
      if (row_pos >= columns)
        row_pos = 0;
      src++;
    }
  IFDBG(3)  g_debug ("End 1 bit conversion");
}