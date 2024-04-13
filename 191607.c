host_scan_for_local_hosts(host_item *host, host_item **lastptr, BOOL *removed)
{
int yield = HOST_FIND_FAILED;
host_item *last = *lastptr;
host_item *prev = NULL;
host_item *h;

if (removed != NULL) *removed = FALSE;

if (local_interface_data == NULL) local_interface_data = host_find_interfaces();

for (h = host; h != last->next; h = h->next)
  {
  #ifndef STAND_ALONE
  if (hosts_treat_as_local != NULL)
    {
    int rc;
    const uschar *save = deliver_domain;
    deliver_domain = h->name;   /* set $domain */
    rc = match_isinlist(string_copylc(h->name), CUSS &hosts_treat_as_local, 0,
      &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL);
    deliver_domain = save;
    if (rc == OK) goto FOUND_LOCAL;
    }
  #endif

  /* It seems that on many operating systems, 0.0.0.0 is treated as a synonym
  for 127.0.0.1 and refers to the local host. We therefore force it always to
  be treated as local. */

  if (h->address != NULL)
    {
    if (Ustrcmp(h->address, "0.0.0.0") == 0) goto FOUND_LOCAL;
    for (ip_address_item * ip = local_interface_data; ip; ip = ip->next)
      if (Ustrcmp(h->address, ip->address) == 0) goto FOUND_LOCAL;
    yield = HOST_FOUND;  /* At least one remote address has been found */
    }

  /* Update prev to point to the last host item before any that have
  the same MX value as the one we have just considered. */

  if (h->next == NULL || h->next->mx != h->mx) prev = h;
  }

return yield;  /* No local hosts found: return HOST_FOUND or HOST_FIND_FAILED */

/* A host whose IP address matches a local IP address, or whose name matches
something in hosts_treat_as_local has been found. */

FOUND_LOCAL:

if (prev == NULL)
  {
  HDEBUG(D_host_lookup) debug_printf((h->mx >= 0)?
    "local host has lowest MX\n" :
    "local host found for non-MX address\n");
  return HOST_FOUND_LOCAL;
  }

HDEBUG(D_host_lookup)
  {
  debug_printf("local host in host list - removed hosts:\n");
  for (h = prev->next; h != last->next; h = h->next)
    debug_printf("  %s %s %d\n", h->name, h->address, h->mx);
  }

if (removed != NULL) *removed = TRUE;
prev->next = last->next;
*lastptr = prev;
return yield;
}