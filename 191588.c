host_build_ifacelist(const uschar *list, uschar *name)
{
int sep = 0;
uschar *s;
ip_address_item * yield = NULL, * last = NULL, * next;
BOOL taint = is_tainted(list);

while ((s = string_nextinlist(&list, &sep, NULL, 0)))
  {
  int ipv;
  int port = host_address_extract_port(s);            /* Leaves just the IP address */

  if (!(ipv = string_is_ip_address(s, NULL)))
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Malformed IP address \"%s\" in %s",
      s, name);

  /* Skip IPv6 addresses if IPv6 is disabled. */

  if (disable_ipv6 && ipv == 6) continue;

  /* This use of strcpy() is OK because we have checked that s is a valid IP
  address above. The field in the ip_address_item is large enough to hold an
  IPv6 address. */

  next = store_get(sizeof(ip_address_item), taint);
  next->next = NULL;
  Ustrcpy(next->address, s);
  next->port = port;
  next->v6_include_v4 = FALSE;
  next->log = NULL;

  if (!yield)
    yield = last = next;
  else
    {
    last->next = next;
    last = next;
    }
  }

return yield;
}