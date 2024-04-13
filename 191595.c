set_address_from_dns(host_item *host, host_item **lastptr,
  const uschar *ignore_target_hosts, BOOL allow_ip,
  const uschar **fully_qualified_name,
  BOOL dnssec_request, BOOL dnssec_require, int whichrrs)
{
host_item *thishostlast = NULL;    /* Indicates not yet filled in anything */
BOOL v6_find_again = FALSE;
BOOL dnssec_fail = FALSE;
int i;
dns_answer * dnsa;

#ifndef DISABLE_TLS
/* Copy the host name at this point to the value which is used for
TLS certificate name checking, before any CNAME-following modifies it.  */

host->certname = host->name;
#endif

/* If allow_ip is set, a name which is an IP address returns that value
as its address. This is used for MX records when allow_mx_to_ip is set, for
those sites that feel they have to flaunt the RFC rules. */

if (allow_ip && string_is_ip_address(host->name, NULL) != 0)
  {
  #ifndef STAND_ALONE
  if (  ignore_target_hosts
     && verify_check_this_host(&ignore_target_hosts, NULL, host->name,
        host->name, NULL) == OK)
    return HOST_IGNORED;
  #endif

  host->address = host->name;
  return HOST_FOUND;
  }

dnsa = store_get_dns_answer();

/* On an IPv6 system, unless IPv6 is disabled, go round the loop up to twice,
looking for AAAA records the first time. However, unless doing standalone
testing, we force an IPv4 lookup if the domain matches dns_ipv4_lookup global.
On an IPv4 system, go round the loop once only, looking only for A records. */

#if HAVE_IPV6
  #ifndef STAND_ALONE
    if (  disable_ipv6
       || !(whichrrs & HOST_FIND_BY_AAAA)
       ||    dns_ipv4_lookup
          && match_isinlist(host->name, CUSS &dns_ipv4_lookup, 0,
	      &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) == OK
       )
      i = 0;    /* look up A records only */
    else
  #endif        /* STAND_ALONE */

  i = 1;        /* look up AAAA and A records */

/* The IPv4 world */

#else           /* HAVE_IPV6 */
  i = 0;        /* look up A records only */
#endif          /* HAVE_IPV6 */

for (; i >= 0; i--)
  {
  static int types[] = { T_A, T_AAAA };
  int type = types[i];
  int randoffset = i == (whichrrs & HOST_FIND_IPV4_FIRST ? 1 : 0)
    ? 500 : 0;  /* Ensures v6/4 sort order */
  dns_scan dnss;

  int rc = dns_lookup_timerwrap(dnsa, host->name, type, fully_qualified_name);
  lookup_dnssec_authenticated = !dnssec_request ? NULL
    : dns_is_secure(dnsa) ? US"yes" : US"no";

  DEBUG(D_dns)
    if (  (dnssec_request || dnssec_require)
       && !dns_is_secure(dnsa)
       && dns_is_aa(dnsa)
       )
      debug_printf("DNS lookup of %.256s (A/AAAA) requested AD, but got AA\n", host->name);

  /* We want to return HOST_FIND_AGAIN if one of the A or AAAA lookups
  fails or times out, but not if another one succeeds. (In the early
  IPv6 days there are name servers that always fail on AAAA, but are happy
  to give out an A record. We want to proceed with that A record.) */

  if (rc != DNS_SUCCEED)
    {
    if (i == 0)  /* Just tried for an A record, i.e. end of loop */
      {
      if (host->address != NULL)
        i = HOST_FOUND;  /* AAAA was found */
      else if (rc == DNS_AGAIN || rc == DNS_FAIL || v6_find_again)
        i = HOST_FIND_AGAIN;
      else
	i = HOST_FIND_FAILED;    /* DNS_NOMATCH or DNS_NODATA */
      goto out;
      }

    /* Tried for an AAAA record: remember if this was a temporary
    error, and look for the next record type. */

    if (rc != DNS_NOMATCH && rc != DNS_NODATA) v6_find_again = TRUE;
    continue;
    }

  if (dnssec_request)
    {
    if (dns_is_secure(dnsa))
      {
      DEBUG(D_host_lookup) debug_printf("%s A DNSSEC\n", host->name);
      if (host->dnssec == DS_UNK) /* set in host_find_bydns() */
	host->dnssec = DS_YES;
      }
    else
      {
      if (dnssec_require)
	{
	dnssec_fail = TRUE;
	DEBUG(D_host_lookup) debug_printf("dnssec fail on %s for %.256s",
		i>0 ? "AAAA" : "A", host->name);
	continue;
	}
      if (host->dnssec == DS_YES) /* set in host_find_bydns() */
	{
	DEBUG(D_host_lookup) debug_printf("%s A cancel DNSSEC\n", host->name);
	host->dnssec = DS_NO;
	lookup_dnssec_authenticated = US"no";
	}
      }
    }

  /* Lookup succeeded: fill in the given host item with the first non-ignored
  address found; create additional items for any others. A single A6 record
  may generate more than one address.  The lookup had a chance to update the
  fqdn; we do not want any later times round the loop to do so. */

  fully_qualified_name = NULL;

  for (dns_record * rr = dns_next_rr(dnsa, &dnss, RESET_ANSWERS);
       rr;
       rr = dns_next_rr(dnsa, &dnss, RESET_NEXT)) if (rr->type == type)
    {
    dns_address * da = dns_address_from_rr(dnsa, rr);

    DEBUG(D_host_lookup)
      if (!da) debug_printf("no addresses extracted from A6 RR for %s\n",
	  host->name);

    /* This loop runs only once for A and AAAA records, but may run
    several times for an A6 record that generated multiple addresses. */

    for (; da; da = da->next)
      {
      #ifndef STAND_ALONE
      if (ignore_target_hosts != NULL &&
	    verify_check_this_host(&ignore_target_hosts, NULL,
	      host->name, da->address, NULL) == OK)
	{
	DEBUG(D_host_lookup)
	  debug_printf("ignored host %s [%s]\n", host->name, da->address);
	continue;
	}
      #endif

      /* If this is the first address, stick it in the given host block,
      and change the name if the returned RR has a different name. */

      if (thishostlast == NULL)
	{
	if (strcmpic(host->name, rr->name) != 0)
	  host->name = string_copy_dnsdomain(rr->name);
	host->address = da->address;
	host->sort_key = host->mx * 1000 + random_number(500) + randoffset;
	host->status = hstatus_unknown;
	host->why = hwhy_unknown;
	thishostlast = host;
	}

      /* Not the first address. Check for, and ignore, duplicates. Then
      insert in the chain at a random point. */

      else
	{
	int new_sort_key;
	host_item *next;

	/* End of our local chain is specified by "thishostlast". */

	for (next = host;; next = next->next)
	  {
	  if (Ustrcmp(CS da->address, next->address) == 0) break;
	  if (next == thishostlast) { next = NULL; break; }
	  }
	if (next != NULL) continue;  /* With loop for next address */

	/* Not a duplicate */

	new_sort_key = host->mx * 1000 + random_number(500) + randoffset;
	next = store_get(sizeof(host_item), FALSE);

	/* New address goes first: insert the new block after the first one
	(so as not to disturb the original pointer) but put the new address
	in the original block. */

	if (new_sort_key < host->sort_key)
	  {
	  *next = *host;                                  /* Copies port */
	  host->next = next;
	  host->address = da->address;
	  host->sort_key = new_sort_key;
	  if (thishostlast == host) thishostlast = next;  /* Local last */
	  if (*lastptr == host) *lastptr = next;          /* Global last */
	  }

	/* Otherwise scan down the addresses for this host to find the
	one to insert after. */

	else
	  {
	  host_item *h = host;
	  while (h != thishostlast)
	    {
	    if (new_sort_key < h->next->sort_key) break;
	    h = h->next;
	    }
	  *next = *h;                                 /* Copies port */
	  h->next = next;
	  next->address = da->address;
	  next->sort_key = new_sort_key;
	  if (h == thishostlast) thishostlast = next; /* Local last */
	  if (h == *lastptr) *lastptr = next;         /* Global last */
	  }
	}
      }
    }
  }

/* Control gets here only if the second lookup (the A record) succeeded.
However, the address may not be filled in if it was ignored. */

i = host->address
  ? HOST_FOUND
  : dnssec_fail
  ? HOST_FIND_SECURITY
  : HOST_IGNORED;

out:
  store_free_dns_answer(dnsa);
  return i;
}