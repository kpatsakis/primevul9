host_nmtoa(int count, int *binary, int mask, uschar *buffer, int sep)
{
int j;
uschar *tt = buffer;

if (count == 1)
  {
  j = binary[0];
  for (int i = 24; i >= 0; i -= 8)
    tt += sprintf(CS tt, "%d.", (j >> i) & 255);
  }
else
  for (int i = 0; i < 4; i++)
    {
    j = binary[i];
    tt += sprintf(CS tt, "%04x%c%04x%c", (j >> 16) & 0xffff, sep, j & 0xffff, sep);
    }

tt--;   /* lose final separator */

if (mask < 0)
  *tt = 0;
else
  tt += sprintf(CS tt, "/%d", mask);

return tt - buffer;
}