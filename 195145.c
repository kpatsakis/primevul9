toString (char *p, size_t len)
{
static uschar buf[1024];

assert (len + 1 < sizeof buf);

memcpy (buf, p, len);
buf[len] = 0;
return buf;
}