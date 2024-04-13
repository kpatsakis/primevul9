convert_16_bit (const gchar *src,
                gchar       *dst,
                guint32     len)
{
/* Convert 16 bit to 8 bit dropping low byte
*/
  gint      i;

  IFDBG(3)  g_debug ("Start 16 bit conversion");

  for (i = 0; i < len >> 1; ++i)
    {
      *dst = *src;
      dst++;
      src += 2;
    }

  IFDBG(3)  g_debug ("End 16 bit conversion");
}