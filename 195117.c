dumpRaw (FILE * fp, uschar *buf, size_t len)
{
int i;

for (i = 0; i < len; ++i)
  fprintf (fp, "%02x ", buf[i]);

fprintf (fp, "\n");
}