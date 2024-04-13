concat (char *out, char *in1, char *in2, int l1, int l2)
{
while (l1--)
  *out++ = *in1++;
while (l2--)
  *out++ = *in2++;
}