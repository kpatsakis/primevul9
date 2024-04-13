host_is_in_net(const uschar *host, const uschar *net, int maskoffset)
{
int address[4];
int incoming[4];
int mlen;
int size = host_aton(net, address);
int insize;

/* No mask => all bits to be checked */

if (maskoffset == 0) mlen = 99999;    /* Big number */
  else mlen = Uatoi(net + maskoffset + 1);

/* Convert the incoming address to binary. */

insize = host_aton(host, incoming);

/* Convert IPv4 addresses given in IPv6 compatible mode, which represent
   connections from IPv4 hosts to IPv6 hosts, that is, addresses of the form
   ::ffff:<v4address>, to IPv4 format. */

if (insize == 4 && incoming[0] == 0 && incoming[1] == 0 &&
    incoming[2] == 0xffff)
  {
  insize = 1;
  incoming[0] = incoming[3];
  }

/* No match if the sizes don't agree. */

if (insize != size) return FALSE;

/* Else do the masked comparison. */

for (int i = 0; i < size; i++)
  {
  int mask;
  if (mlen == 0) mask = 0;
  else if (mlen < 32)
    {
    mask = (uint)(-1) << (32 - mlen);
    mlen = 0;
    }
  else
    {
    mask = -1;
    mlen -= 32;
    }
  if ((incoming[i] & mask) != (address[i] & mask)) return FALSE;
  }

return TRUE;
}