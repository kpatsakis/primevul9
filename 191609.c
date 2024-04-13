int main(int argc, char **cargv)
{
host_item h;
int whichrrs = HOST_FIND_BY_MX | HOST_FIND_BY_A | HOST_FIND_BY_AAAA;
BOOL byname = FALSE;
BOOL qualify_single = TRUE;
BOOL search_parents = FALSE;
BOOL request_dnssec = FALSE;
BOOL require_dnssec = FALSE;
uschar **argv = USS cargv;
uschar buffer[256];

disable_ipv6 = FALSE;
primary_hostname = US"";
store_init();
store_pool = POOL_MAIN;
debug_selector = D_host_lookup|D_interface;
debug_file = stdout;
debug_fd = fileno(debug_file);

printf("Exim stand-alone host functions test\n");

host_find_interfaces();
debug_selector = D_host_lookup | D_dns;

if (argc > 1) primary_hostname = argv[1];

/* So that debug level changes can be done first */

dns_init(qualify_single, search_parents, FALSE);

printf("Testing host lookup\n");
printf("> ");
while (Ufgets(buffer, 256, stdin) != NULL)
  {
  int rc;
  int len = Ustrlen(buffer);
  uschar *fully_qualified_name;

  while (len > 0 && isspace(buffer[len-1])) len--;
  buffer[len] = 0;

  if (Ustrcmp(buffer, "q") == 0) break;

  if (Ustrcmp(buffer, "byname") == 0) byname = TRUE;
  else if (Ustrcmp(buffer, "no_byname") == 0) byname = FALSE;
  else if (Ustrcmp(buffer, "a_only") == 0) whichrrs = HOST_FIND_BY_A | HOST_FIND_BY_AAAA;
  else if (Ustrcmp(buffer, "mx_only") == 0) whichrrs = HOST_FIND_BY_MX;
  else if (Ustrcmp(buffer, "srv_only") == 0) whichrrs = HOST_FIND_BY_SRV;
  else if (Ustrcmp(buffer, "srv+a") == 0)
    whichrrs = HOST_FIND_BY_SRV | HOST_FIND_BY_A | HOST_FIND_BY_AAAA;
  else if (Ustrcmp(buffer, "srv+mx") == 0)
    whichrrs = HOST_FIND_BY_SRV | HOST_FIND_BY_MX;
  else if (Ustrcmp(buffer, "srv+mx+a") == 0)
    whichrrs = HOST_FIND_BY_SRV | HOST_FIND_BY_MX | HOST_FIND_BY_A | HOST_FIND_BY_AAAA;
  else if (Ustrcmp(buffer, "qualify_single")    == 0) qualify_single = TRUE;
  else if (Ustrcmp(buffer, "no_qualify_single") == 0) qualify_single = FALSE;
  else if (Ustrcmp(buffer, "search_parents")    == 0) search_parents = TRUE;
  else if (Ustrcmp(buffer, "no_search_parents") == 0) search_parents = FALSE;
  else if (Ustrcmp(buffer, "request_dnssec")    == 0) request_dnssec = TRUE;
  else if (Ustrcmp(buffer, "no_request_dnssec") == 0) request_dnssec = FALSE;
  else if (Ustrcmp(buffer, "require_dnssec")    == 0) require_dnssec = TRUE;
  else if (Ustrcmp(buffer, "no_require_dnssec") == 0) require_dnssec = FALSE;
  else if (Ustrcmp(buffer, "test_harness") == 0)
    f.running_in_test_harness = !f.running_in_test_harness;
  else if (Ustrcmp(buffer, "ipv6") == 0) disable_ipv6 = !disable_ipv6;
  else if (Ustrcmp(buffer, "res_debug") == 0)
    {
    _res.options ^= RES_DEBUG;
    }
  else if (Ustrncmp(buffer, "retrans", 7) == 0)
    {
    (void)sscanf(CS(buffer+8), "%d", &dns_retrans);
    _res.retrans = dns_retrans;
    }
  else if (Ustrncmp(buffer, "retry", 5) == 0)
    {
    (void)sscanf(CS(buffer+6), "%d", &dns_retry);
    _res.retry = dns_retry;
    }
  else
    {
    int flags = whichrrs;
    dnssec_domains d;

    h.name = buffer;
    h.next = NULL;
    h.mx = MX_NONE;
    h.port = PORT_NONE;
    h.status = hstatus_unknown;
    h.why = hwhy_unknown;
    h.address = NULL;

    if (qualify_single) flags |= HOST_FIND_QUALIFY_SINGLE;
    if (search_parents) flags |= HOST_FIND_SEARCH_PARENTS;

    d.request = request_dnssec ? &h.name : NULL;
    d.require = require_dnssec ? &h.name : NULL;

    rc = byname
      ? host_find_byname(&h, NULL, flags, &fully_qualified_name, TRUE)
      : host_find_bydns(&h, NULL, flags, US"smtp", NULL, NULL,
			&d, &fully_qualified_name, NULL);

    switch (rc)
      {
      case HOST_FIND_FAILED:	printf("Failed\n");	break;
      case HOST_FIND_AGAIN:	printf("Again\n");	break;
      case HOST_FIND_SECURITY:	printf("Security\n");	break;
      case HOST_FOUND_LOCAL:	printf("Local\n");	break;
      }
    }

  printf("\n> ");
  }

printf("Testing host_aton\n");
printf("> ");
while (Ufgets(buffer, 256, stdin) != NULL)
  {
  int x[4];
  int len = Ustrlen(buffer);

  while (len > 0 && isspace(buffer[len-1])) len--;
  buffer[len] = 0;

  if (Ustrcmp(buffer, "q") == 0) break;

  len = host_aton(buffer, x);
  printf("length = %d ", len);
  for (int i = 0; i < len; i++)
    {
    printf("%04x ", (x[i] >> 16) & 0xffff);
    printf("%04x ", x[i] & 0xffff);
    }
  printf("\n> ");
  }

printf("\n");

printf("Testing host_name_lookup\n");
printf("> ");
while (Ufgets(buffer, 256, stdin) != NULL)
  {
  int len = Ustrlen(buffer);
  while (len > 0 && isspace(buffer[len-1])) len--;
  buffer[len] = 0;
  if (Ustrcmp(buffer, "q") == 0) break;
  sender_host_address = buffer;
  sender_host_name = NULL;
  sender_host_aliases = NULL;
  host_lookup_msg = US"";
  host_lookup_failed = FALSE;
  if (host_name_lookup() == FAIL)  /* Debug causes printing */
    printf("Lookup failed:%s\n", host_lookup_msg);
  printf("\n> ");
  }

printf("\n");

return 0;
}