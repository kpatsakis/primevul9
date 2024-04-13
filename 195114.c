smbhash (uschar *out, uschar *in, uschar *key, int forw)
{
int i;
char outb[64];
char inb[64];
char keyb[64];
uschar key2[8];

str_to_key (key, key2);

for (i = 0; i < 64; i++)
  {
  inb[i] = (in[i / 8] & (1 << (7 - (i % 8)))) ? 1 : 0;
  keyb[i] = (key2[i / 8] & (1 << (7 - (i % 8)))) ? 1 : 0;
  outb[i] = 0;
  }

dohash (outb, inb, keyb, forw);

for (i = 0; i < 8; i++)
  out[i] = 0;

for (i = 0; i < 64; i++)
  if (outb[i])
   out[i / 8] |= (1 << (7 - (i % 8)));
}