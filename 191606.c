ipv6_nmtoa(int * binary, uschar * buffer)
{
int i, j, k;
uschar * c = buffer;
uschar * d = NULL;	/* shut insufficiently "clever" compiler up */

for (i = 0; i < 4; i++)
  {			/* expand to text */
  j = binary[i];
  c += sprintf(CS c, "%x:%x:", (j >> 16) & 0xffff, j & 0xffff);
  }

for (c = buffer, k = -1, i = 0; i < 8; i++)
  {			/* find longest 0-group sequence */
  if (*c == '0')	/* must be "0:" */
    {
    uschar * s = c;
    j = i;
    while (c[2] == '0') i++, c += 2;
    if (i-j > k)
      {
      k = i-j;		/* length of sequence */
      d = s;		/* start of sequence */
      }
    }
  while (*++c != ':') ;
  c++;
  }

*--c = '\0';	/* drop trailing colon */

/* debug_printf("%s: D k %d <%s> <%s>\n", __FUNCTION__, k, buffer, buffer + 2*(k+1)); */
if (k >= 0)
  {			/* collapse */
  c = d + 2*(k+1);
  if (d == buffer) c--;	/* need extra colon */
  *d++ = ':';	/* 1st 0 */
  while ((*d++ = *c++)) ;
  }
else
  d = c;

return d - buffer;
}