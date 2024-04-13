unicodeToString (char *p, size_t len)
{
int i;
static char buf[1024];

assert (len + 1 < sizeof buf);

for (i = 0; i < len; ++i)
  {
  buf[i] = *p & 0x7f;
  p += 2;
  }

buf[i] = '\0';
return buf;
}