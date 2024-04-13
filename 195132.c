_my_mbstowcs (int16x * dst, uschar * src, int len)
{
int i;
int16x val;

for (i = 0; i < len; i++)
  {
  val = *src;
  SSVAL (dst, 0, val);
  dst++;
  src++;
  if (val == 0)
   break;
  }
return i;
}