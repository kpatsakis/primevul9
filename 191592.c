host_find_byname(host_item *host, const uschar *ignore_target_hosts, int flags,
  const uschar **fully_qualified_name, BOOL local_host_check)
{
int yield, times;
host_item *last = NULL;
BOOL temp_error = FALSE;
int af;

#ifndef DISABLE_TLS
/* Copy the host name at this point to the value which is used for
TLS certificate name checking, before anything modifies it.  */

host->certname = host->name;
#endif

/* Make sure DNS options are set as required. This appears to be necessary in
some circumstances when the get..byname() function actually calls the DNS. */

dns_init((flags & HOST_FIND_QUALIFY_SINGLE) != 0,
         (flags & HOST_FIND_SEARCH_PARENTS) != 0,
	 FALSE);		/* Cannot retrieve dnssec status so do not request */

/* In an IPv6 world, unless IPv6 has been disabled, we need to scan for both
kinds of address, so go round the loop twice. Note that we have ensured that
AF_INET6 is defined even in an IPv4 world, which makes for slightly tidier
code. However, if dns_ipv4_lookup matches the domain, we also just do IPv4
lookups here (except when testing standalone). */

#if HAVE_IPV6
  #ifdef STAND_ALONE
  if (disable_ipv6)
  #else
  if (  disable_ipv6
     ||    dns_ipv4_lookup
	&& match_isinlist(host->name, CUSS &dns_ipv4_lookup, 0,
	    &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) == OK)
  #endif

    { af = AF_INET; times = 1; }
  else
    { af = AF_INET6; times = 2; }

/* No IPv6 support */

#else   /* HAVE_IPV6 */
  af = AF_INET; times = 1;
#endif  /* HAVE_IPV6 */

/* Initialize the flag that gets set for DNS syntax check errors, so that the
interface to this function can be similar to host_find_bydns. */

f.host_find_failed_syntax = FALSE;

/* Loop to look up both kinds of address in an IPv6 world */

for (int i = 1; i <= times;
     #if HAVE_IPV6
       af = AF_INET,     /* If 2 passes, IPv4 on the second */
     #endif
     i++)
  {
  BOOL ipv4_addr;
  int error_num = 0;
  struct hostent *hostdata;
  unsigned long time_msec = 0;	/* compiler quietening */

  #ifdef STAND_ALONE
  printf("Looking up: %s\n", host->name);
  #endif

  if (slow_lookup_log) time_msec = get_time_in_ms();

  #if HAVE_IPV6
  if (f.running_in_test_harness)
    hostdata = host_fake_gethostbyname(host->name, af, &error_num);
  else
    {
    #if HAVE_GETIPNODEBYNAME
    hostdata = getipnodebyname(CS host->name, af, 0, &error_num);
    #else
    hostdata = gethostbyname2(CS host->name, af);
    error_num = h_errno;
    #endif
    }

  #else    /* not HAVE_IPV6 */
  if (f.running_in_test_harness)
    hostdata = host_fake_gethostbyname(host->name, af, &error_num);
  else
    {
    hostdata = gethostbyname(CS host->name);
    error_num = h_errno;
    }
  #endif   /* HAVE_IPV6 */

  if (   slow_lookup_log
      && (time_msec = get_time_in_ms() - time_msec) > slow_lookup_log)
    log_long_lookup(US"gethostbyname", host->name, time_msec);

  if (!hostdata)
    {
    uschar * error;
    switch (error_num)
      {
      case HOST_NOT_FOUND: error = US"HOST_NOT_FOUND";	break;
      case TRY_AGAIN:      error = US"TRY_AGAIN";   temp_error = TRUE; break;
      case NO_RECOVERY:    error = US"NO_RECOVERY"; temp_error = TRUE; break;
      case NO_DATA:        error = US"NO_DATA";		break;
    #if NO_DATA != NO_ADDRESS
      case NO_ADDRESS:     error = US"NO_ADDRESS";	break;
    #endif
      default: error = US"?"; break;
      }

    DEBUG(D_host_lookup) debug_printf("%s(af=%s) returned %d (%s)\n",
      f.running_in_test_harness ? "host_fake_gethostbyname" :
#if HAVE_IPV6
# if HAVE_GETIPNODEBYNAME
        "getipnodebyname",
# else
        "gethostbyname2",
# endif
#else
	"gethostbyname",
#endif
      af == AF_INET ? "inet" : "inet6", error_num, error);

    continue;
    }
  if (!(hostdata->h_addr_list)[0]) continue;

  /* Replace the name with the fully qualified one if necessary, and fill in
  the fully_qualified_name pointer. */

  if (hostdata->h_name[0] && Ustrcmp(host->name, hostdata->h_name) != 0)
    host->name = string_copy_dnsdomain(US hostdata->h_name);
  if (fully_qualified_name) *fully_qualified_name = host->name;

  /* Get the list of addresses. IPv4 and IPv6 addresses can be distinguished
  by their different lengths. Scan the list, ignoring any that are to be
  ignored, and build a chain from the rest. */

  ipv4_addr = hostdata->h_length == sizeof(struct in_addr);

  for (uschar ** addrlist = USS hostdata->h_addr_list; *addrlist; addrlist++)
    {
    uschar *text_address =
      host_ntoa(ipv4_addr? AF_INET:AF_INET6, *addrlist, NULL, NULL);

    #ifndef STAND_ALONE
    if (  ignore_target_hosts
       && verify_check_this_host(&ignore_target_hosts, NULL, host->name,
	    text_address, NULL) == OK)
      {
      DEBUG(D_host_lookup)
        debug_printf("ignored host %s [%s]\n", host->name, text_address);
      continue;
      }
    #endif

    /* If this is the first address, last is NULL and we put the data in the
    original block. */

    if (!last)
      {
      host->address = text_address;
      host->port = PORT_NONE;
      host->status = hstatus_unknown;
      host->why = hwhy_unknown;
      host->dnssec = DS_UNK;
      last = host;
      }

    /* Else add further host item blocks for any other addresses, keeping
    the order. */

    else
      {
      host_item *next = store_get(sizeof(host_item), FALSE);
      next->name = host->name;
#ifndef DISABLE_TLS
      next->certname = host->certname;
#endif
      next->mx = host->mx;
      next->address = text_address;
      next->port = PORT_NONE;
      next->status = hstatus_unknown;
      next->why = hwhy_unknown;
      next->dnssec = DS_UNK;
      next->last_try = 0;
      next->next = last->next;
      last->next = next;
      last = next;
      }
    }
  }

/* If no hosts were found, the address field in the original host block will be
NULL. If temp_error is set, at least one of the lookups gave a temporary error,
so we pass that back. */

if (!host->address)
  {
  uschar *msg =
    #ifndef STAND_ALONE
    !message_id[0] && smtp_in
      ? string_sprintf("no IP address found for host %s (during %s)", host->name,
          smtp_get_connection_info()) :
    #endif
    string_sprintf("no IP address found for host %s", host->name);

  HDEBUG(D_host_lookup) debug_printf("%s\n", msg);
  if (temp_error) goto RETURN_AGAIN;
  if (host_checking || !f.log_testing_mode)
    log_write(L_host_lookup_failed, LOG_MAIN, "%s", msg);
  return HOST_FIND_FAILED;
  }

/* Remove any duplicate IP addresses, then check to see if this is the local
host if required. */

host_remove_duplicates(host, &last);
yield = local_host_check?
  host_scan_for_local_hosts(host, &last, NULL) : HOST_FOUND;

HDEBUG(D_host_lookup)
  {
  if (fully_qualified_name)
    debug_printf("fully qualified name = %s\n", *fully_qualified_name);
  debug_printf("%s looked up these IP addresses:\n",
    #if HAVE_IPV6
      #if HAVE_GETIPNODEBYNAME
      "getipnodebyname"
      #else
      "gethostbyname2"
      #endif
    #else
    "gethostbyname"
    #endif
    );
  for (const host_item * h = host; h != last->next; h = h->next)
    debug_printf("  name=%s address=%s\n", h->name,
      h->address ? h->address : US"<null>");
  }

/* Return the found status. */

return yield;

/* Handle the case when there is a temporary error. If the name matches
dns_again_means_nonexist, return permanent rather than temporary failure. */

RETURN_AGAIN:
  {
#ifndef STAND_ALONE
  int rc;
  const uschar *save = deliver_domain;
  deliver_domain = host->name;  /* set $domain */
  rc = match_isinlist(host->name, CUSS &dns_again_means_nonexist, 0,
    &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL);
  deliver_domain = save;
  if (rc == OK)
    {
    DEBUG(D_host_lookup) debug_printf("%s is in dns_again_means_nonexist: "
      "returning HOST_FIND_FAILED\n", host->name);
    return HOST_FIND_FAILED;
    }
#endif
  return HOST_FIND_AGAIN;
  }
}