host_ntoa(int type, const void *arg, uschar *buffer, int *portptr)
{
uschar *yield;

/* The new world. It is annoying that we have to fish out the address from
different places in the block, depending on what kind of address it is. It
is also a pain that inet_ntop() returns a const uschar *, whereas the IPv4
function inet_ntoa() returns just uschar *, and some picky compilers insist
on warning if one assigns a const uschar * to a uschar *. Hence the casts. */

#if HAVE_IPV6
uschar addr_buffer[46];
if (type < 0)
  {
  int family = ((struct sockaddr *)arg)->sa_family;
  if (family == AF_INET6)
    {
    struct sockaddr_in6 *sk = (struct sockaddr_in6 *)arg;
    yield = US inet_ntop(family, &(sk->sin6_addr), CS addr_buffer,
      sizeof(addr_buffer));
    if (portptr != NULL) *portptr = ntohs(sk->sin6_port);
    }
  else
    {
    struct sockaddr_in *sk = (struct sockaddr_in *)arg;
    yield = US inet_ntop(family, &(sk->sin_addr), CS addr_buffer,
      sizeof(addr_buffer));
    if (portptr != NULL) *portptr = ntohs(sk->sin_port);
    }
  }
else
  {
  yield = US inet_ntop(type, arg, CS addr_buffer, sizeof(addr_buffer));
  }

/* If the result is a mapped IPv4 address, show it in V4 format. */

if (Ustrncmp(yield, "::ffff:", 7) == 0) yield += 7;

#else  /* HAVE_IPV6 */

/* The old world */

if (type < 0)
  {
  yield = US inet_ntoa(((struct sockaddr_in *)arg)->sin_addr);
  if (portptr != NULL) *portptr = ntohs(((struct sockaddr_in *)arg)->sin_port);
  }
else
  yield = US inet_ntoa(*((struct in_addr *)arg));
#endif

/* If there is no buffer, put the string into some new store. */

if (!buffer) buffer = store_get(46, FALSE);

/* Callers of this function with a non-NULL buffer must ensure that it is
large enough to hold an IPv6 address, namely, at least 46 bytes. That's what
makes this use of strcpy() OK.
If the library returned apparently an apparently tainted string, clean it;
we trust IP addresses. */

string_format_nt(buffer, 46, "%s", yield);
return buffer;
}