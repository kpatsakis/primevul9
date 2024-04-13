strToUnicode (char *p)
{
static uschar buf[1024];
size_t l = strlen (p);
int i = 0;

assert (l * 2 < sizeof buf);

while (l--)
  {
  buf[i++] = *p++;
  buf[i++] = 0;
  }

return buf;
}