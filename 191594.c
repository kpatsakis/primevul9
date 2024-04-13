host_mask(int count, int *binary, int mask)
{
if (mask < 0) mask = 99999;
for (int i = 0; i < count; i++)
  {
  int wordmask;
  if (mask == 0) wordmask = 0;
  else if (mask < 32)
    {
    wordmask = (uint)(-1) << (32 - mask);
    mask = 0;
    }
  else
    {
    wordmask = -1;
    mask -= 32;
    }
  binary[i] &= wordmask;
  }
}