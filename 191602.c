host_find_bydns(host_item *host, const uschar *ignore_target_hosts, int whichrrs,
  uschar *srv_service, uschar *srv_fail_domains, uschar *mx_fail_domains,
  const dnssec_domains *dnssec_d,
  const uschar **fully_qualified_name, BOOL *removed)
{
host_item *h, *last;
int rc = DNS_FAIL;
int ind_type = 0;
int yield;
dns_answer * dnsa = store_get_dns_answer();
dns_scan dnss;
BOOL dnssec_require = dnssec_d
  && match_isinlist(host->name, CUSS &dnssec_d->require,
		  0, &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) == OK;
BOOL dnssec_request = dnssec_require
    || (  dnssec_d
       && match_isinlist(host->name, CUSS &dnssec_d->request,
		    0, &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) == OK);
dnssec_status_t dnssec;

/* Set the default fully qualified name to the incoming name, initialize the
resolver if necessary, set up the relevant options, and initialize the flag
that gets set for DNS syntax check errors. */

if (fully_qualified_name != NULL) *fully_qualified_name = host->name;
dns_init((whichrrs & HOST_FIND_QUALIFY_SINGLE) != 0,
         (whichrrs & HOST_FIND_SEARCH_PARENTS) != 0,
	 dnssec_request);
f.host_find_failed_syntax = FALSE;

/* First, if requested, look for SRV records. The service name is given; we
assume TCP protocol. DNS domain names are constrained to a maximum of 256
characters, so the code below should be safe. */

if (whichrrs & HOST_FIND_BY_SRV)
  {
  gstring * g;
  uschar * temp_fully_qualified_name;
  int prefix_length;

  g = string_fmt_append(NULL, "_%s._tcp.%n%.256s",
	srv_service, &prefix_length, host->name);
  temp_fully_qualified_name = string_from_gstring(g);
  ind_type = T_SRV;

  /* Search for SRV records. If the fully qualified name is different to
  the input name, pass back the new original domain, without the prepended
  magic. */

  dnssec = DS_UNK;
  lookup_dnssec_authenticated = NULL;
  rc = dns_lookup_timerwrap(dnsa, temp_fully_qualified_name, ind_type,
	CUSS &temp_fully_qualified_name);

  DEBUG(D_dns)
    if ((dnssec_request || dnssec_require)
	&& !dns_is_secure(dnsa)
	&& dns_is_aa(dnsa))
      debug_printf("DNS lookup of %.256s (SRV) requested AD, but got AA\n", host->name);

  if (dnssec_request)
    {
    if (dns_is_secure(dnsa))
      { dnssec = DS_YES; lookup_dnssec_authenticated = US"yes"; }
    else
      { dnssec = DS_NO; lookup_dnssec_authenticated = US"no"; }
    }

  if (temp_fully_qualified_name != g->s && fully_qualified_name != NULL)
    *fully_qualified_name = temp_fully_qualified_name + prefix_length;

  /* On DNS failures, we give the "try again" error unless the domain is
  listed as one for which we continue. */

  if (rc == DNS_SUCCEED && dnssec_require && !dns_is_secure(dnsa))
    {
    log_write(L_host_lookup_failed, LOG_MAIN,
		"dnssec fail on SRV for %.256s", host->name);
    rc = DNS_FAIL;
    }
  if (rc == DNS_FAIL || rc == DNS_AGAIN)
    {
#ifndef STAND_ALONE
    if (match_isinlist(host->name, CUSS &srv_fail_domains, 0,
	&domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) != OK)
#endif
      { yield = HOST_FIND_AGAIN; goto out; }
    DEBUG(D_host_lookup) debug_printf("DNS_%s treated as DNS_NODATA "
      "(domain in srv_fail_domains)\n", rc == DNS_FAIL ? "FAIL":"AGAIN");
    }
  }

/* If we did not find any SRV records, search the DNS for MX records, if
requested to do so. If the result is DNS_NOMATCH, it means there is no such
domain, and there's no point in going on to look for address records with the
same domain. The result will be DNS_NODATA if the domain exists but has no MX
records. On DNS failures, we give the "try again" error unless the domain is
listed as one for which we continue. */

if (rc != DNS_SUCCEED  &&  whichrrs & HOST_FIND_BY_MX)
  {
  ind_type = T_MX;
  dnssec = DS_UNK;
  lookup_dnssec_authenticated = NULL;
  rc = dns_lookup_timerwrap(dnsa, host->name, ind_type, fully_qualified_name);

  DEBUG(D_dns)
    if (  (dnssec_request || dnssec_require)
       && !dns_is_secure(dnsa)
       && dns_is_aa(dnsa))
      debug_printf("DNS lookup of %.256s (MX) requested AD, but got AA\n", host->name);

  if (dnssec_request)
    if (dns_is_secure(dnsa))
      {
      DEBUG(D_host_lookup) debug_printf("%s (MX resp) DNSSEC\n", host->name);
      dnssec = DS_YES; lookup_dnssec_authenticated = US"yes";
      }
    else
      {
      dnssec = DS_NO; lookup_dnssec_authenticated = US"no";
      }

  switch (rc)
    {
    case DNS_NOMATCH:
      yield = HOST_FIND_FAILED; goto out;

    case DNS_SUCCEED:
      if (!dnssec_require || dns_is_secure(dnsa))
	break;
      DEBUG(D_host_lookup)
	debug_printf("dnssec fail on MX for %.256s", host->name);
#ifndef STAND_ALONE
      if (match_isinlist(host->name, CUSS &mx_fail_domains, 0,
	  &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) != OK)
	{ yield = HOST_FIND_SECURITY; goto out; }
#endif
      rc = DNS_FAIL;
      /*FALLTHROUGH*/

    case DNS_FAIL:
    case DNS_AGAIN:
#ifndef STAND_ALONE
      if (match_isinlist(host->name, CUSS &mx_fail_domains, 0,
	  &domainlist_anchor, NULL, MCL_DOMAIN, TRUE, NULL) != OK)
#endif
	{ yield = HOST_FIND_AGAIN; goto out; }
      DEBUG(D_host_lookup) debug_printf("DNS_%s treated as DNS_NODATA "
	"(domain in mx_fail_domains)\n", (rc == DNS_FAIL)? "FAIL":"AGAIN");
      break;
    }
  }

/* If we haven't found anything yet, and we are requested to do so, try for an
A or AAAA record. If we find it (or them) check to see that it isn't the local
host. */

if (rc != DNS_SUCCEED)
  {
  if (!(whichrrs & (HOST_FIND_BY_A | HOST_FIND_BY_AAAA)))
    {
    DEBUG(D_host_lookup) debug_printf("Address records are not being sought\n");
    yield = HOST_FIND_FAILED;
    goto out;
    }

  last = host;        /* End of local chainlet */
  host->mx = MX_NONE;
  host->port = PORT_NONE;
  host->dnssec = DS_UNK;
  lookup_dnssec_authenticated = NULL;
  rc = set_address_from_dns(host, &last, ignore_target_hosts, FALSE,
    fully_qualified_name, dnssec_request, dnssec_require, whichrrs);

  /* If one or more address records have been found, check that none of them
  are local. Since we know the host items all have their IP addresses
  inserted, host_scan_for_local_hosts() can only return HOST_FOUND or
  HOST_FOUND_LOCAL. We do not need to scan for duplicate IP addresses here,
  because set_address_from_dns() removes them. */

  if (rc == HOST_FOUND)
    rc = host_scan_for_local_hosts(host, &last, removed);
  else
    if (rc == HOST_IGNORED) rc = HOST_FIND_FAILED;  /* No special action */

  DEBUG(D_host_lookup)
    if (host->address)
      {
      if (fully_qualified_name)
        debug_printf("fully qualified name = %s\n", *fully_qualified_name);
      for (host_item * h = host; h != last->next; h = h->next)
        debug_printf("%s %s mx=%d sort=%d %s\n", h->name,
          h->address ? h->address : US"<null>", h->mx, h->sort_key,
          h->status >= hstatus_unusable ? US"*" : US"");
      }

  yield = rc;
  goto out;
  }

/* We have found one or more MX or SRV records. Sort them according to
precedence. Put the data for the first one into the existing host block, and
insert new host_item blocks into the chain for the remainder. For equal
precedences one is supposed to randomize the order. To make this happen, the
sorting is actually done on the MX value * 1000 + a random number. This is put
into a host field called sort_key.

In the case of hosts with both IPv6 and IPv4 addresses, we want to choose the
IPv6 address in preference. At this stage, we don't know what kind of address
the host has. We choose a random number < 500; if later we find an A record
first, we add 500 to the random number. Then for any other address records, we
use random numbers in the range 0-499 for AAAA records and 500-999 for A
records.

At this point we remove any duplicates that point to the same host, retaining
only the one with the lowest precedence. We cannot yet check for precedence
greater than that of the local host, because that test cannot be properly done
until the addresses have been found - an MX record may point to a name for this
host which is not the primary hostname. */

last = NULL;    /* Indicates that not even the first item is filled yet */

for (dns_record * rr = dns_next_rr(dnsa, &dnss, RESET_ANSWERS);
     rr;
     rr = dns_next_rr(dnsa, &dnss, RESET_NEXT)) if (rr->type == ind_type)
  {
  int precedence, weight;
  int port = PORT_NONE;
  const uschar * s = rr->data;	/* MUST be unsigned for GETSHORT */
  uschar data[256];

  GETSHORT(precedence, s);      /* Pointer s is advanced */

  /* For MX records, we use a random "weight" which causes multiple records of
  the same precedence to sort randomly. */

  if (ind_type == T_MX)
    weight = random_number(500);
  else
    {
    /* SRV records are specified with a port and a weight. The weight is used
    in a special algorithm. However, to start with, we just use it to order the
    records of equal priority (precedence). */
    GETSHORT(weight, s);
    GETSHORT(port, s);
    }

  /* Get the name of the host pointed to. */

  (void)dn_expand(dnsa->answer, dnsa->answer + dnsa->answerlen, s,
    (DN_EXPAND_ARG4_TYPE)data, sizeof(data));

  /* Check that we haven't already got this host on the chain; if we have,
  keep only the lower precedence. This situation shouldn't occur, but you
  never know what junk might get into the DNS (and this case has been seen on
  more than one occasion). */

  if (last)       /* This is not the first record */
    {
    host_item *prev = NULL;

    for (h = host; h != last->next; prev = h, h = h->next)
      if (strcmpic(h->name, data) == 0)
        {
        DEBUG(D_host_lookup)
          debug_printf("discarded duplicate host %s (MX=%d)\n", data,
            precedence > h->mx ? precedence : h->mx);
        if (precedence >= h->mx) goto NEXT_MX_RR; /* Skip greater precedence */
        if (h == host)                            /* Override first item */
          {
          h->mx = precedence;
          host->sort_key = precedence * 1000 + weight;
          goto NEXT_MX_RR;
          }

        /* Unwanted host item is not the first in the chain, so we can get
        get rid of it by cutting it out. */

        prev->next = h->next;
        if (h == last) last = prev;
        break;
        }
    }

  /* If this is the first MX or SRV record, put the data into the existing host
  block. Otherwise, add a new block in the correct place; if it has to be
  before the first block, copy the first block's data to a new second block. */

  if (!last)
    {
    host->name = string_copy_dnsdomain(data);
    host->address = NULL;
    host->port = port;
    host->mx = precedence;
    host->sort_key = precedence * 1000 + weight;
    host->status = hstatus_unknown;
    host->why = hwhy_unknown;
    host->dnssec = dnssec;
    last = host;
    }
  else

  /* Make a new host item and seek the correct insertion place */
    {
    int sort_key = precedence * 1000 + weight;
    host_item *next = store_get(sizeof(host_item), FALSE);
    next->name = string_copy_dnsdomain(data);
    next->address = NULL;
    next->port = port;
    next->mx = precedence;
    next->sort_key = sort_key;
    next->status = hstatus_unknown;
    next->why = hwhy_unknown;
    next->dnssec = dnssec;
    next->last_try = 0;

    /* Handle the case when we have to insert before the first item. */

    if (sort_key < host->sort_key)
      {
      host_item htemp;
      htemp = *host;
      *host = *next;
      *next = htemp;
      host->next = next;
      if (last == host) last = next;
      }
    else

    /* Else scan down the items we have inserted as part of this exercise;
    don't go further. */
      {
      for (h = host; h != last; h = h->next)
        if (sort_key < h->next->sort_key)
          {
          next->next = h->next;
          h->next = next;
          break;
          }

      /* Join on after the last host item that's part of this
      processing if we haven't stopped sooner. */

      if (h == last)
        {
        next->next = last->next;
        last->next = next;
        last = next;
        }
      }
    }

  NEXT_MX_RR: continue;
  }

if (!last)	/* No rr of correct type; give up */
  {
  yield = HOST_FIND_FAILED;
  goto out;
  }

/* If the list of hosts was obtained from SRV records, there are two things to
do. First, if there is only one host, and it's name is ".", it means there is
no SMTP service at this domain. Otherwise, we have to sort the hosts of equal
priority according to their weights, using an algorithm that is defined in RFC
2782. The hosts are currently sorted by priority and weight. For each priority
group we have to pick off one host and put it first, and then repeat for any
remaining in the same priority group. */

if (ind_type == T_SRV)
  {
  host_item ** pptr;

  if (host == last && host->name[0] == 0)
    {
    DEBUG(D_host_lookup) debug_printf("the single SRV record is \".\"\n");
    yield = HOST_FIND_FAILED;
    goto out;
    }

  DEBUG(D_host_lookup)
    {
    debug_printf("original ordering of hosts from SRV records:\n");
    for (h = host; h != last->next; h = h->next)
      debug_printf("  %s P=%d W=%d\n", h->name, h->mx, h->sort_key % 1000);
    }

  for (pptr = &host, h = host; h != last; pptr = &h->next, h = h->next)
    {
    int sum = 0;
    host_item *hh;

    /* Find the last following host that has the same precedence. At the same
    time, compute the sum of the weights and the running totals. These can be
    stored in the sort_key field. */

    for (hh = h; hh != last; hh = hh->next)
      {
      int weight = hh->sort_key % 1000;   /* was precedence * 1000 + weight */
      sum += weight;
      hh->sort_key = sum;
      if (hh->mx != hh->next->mx) break;
      }

    /* If there's more than one host at this precedence (priority), we need to
    pick one to go first. */

    if (hh != h)
      {
      host_item *hhh;
      host_item **ppptr;
      int randomizer = random_number(sum + 1);

      for (ppptr = pptr, hhh = h;
           hhh != hh;
           ppptr = &hhh->next, hhh = hhh->next)
        if (hhh->sort_key >= randomizer)
	  break;

      /* hhh now points to the host that should go first; ppptr points to the
      place that points to it. Unfortunately, if the start of the minilist is
      the start of the entire list, we can't just swap the items over, because
      we must not change the value of host, since it is passed in from outside.
      One day, this could perhaps be changed.

      The special case is fudged by putting the new item *second* in the chain,
      and then transferring the data between the first and second items. We
      can't just swap the first and the chosen item, because that would mean
      that an item with zero weight might no longer be first. */

      if (hhh != h)
        {
        *ppptr = hhh->next;          /* Cuts it out of the chain */

        if (h == host)
          {
          host_item temp = *h;
          *h = *hhh;
          *hhh = temp;
          hhh->next = temp.next;
          h->next = hhh;
          }
        else
          {
          hhh->next = h;               /* The rest of the chain follows it */
          *pptr = hhh;                 /* It takes the place of h */
          h = hhh;                     /* It's now the start of this minilist */
          }
        }
      }

    /* A host has been chosen to be first at this priority and h now points
    to this host. There may be others at the same priority, or others at a
    different priority. Before we leave this host, we need to put back a sort
    key of the traditional MX kind, in case this host is multihomed, because
    the sort key is used for ordering the multiple IP addresses. We do not need
    to ensure that these new sort keys actually reflect the order of the hosts,
    however. */

    h->sort_key = h->mx * 1000 + random_number(500);
    }   /* Move on to the next host */
  }

/* Now we have to find IP addresses for all the hosts. We have ensured above
that the names in all the host items are unique. Before release 4.61 we used to
process records from the additional section in the DNS packet that returned the
MX or SRV records. However, a DNS name server is free to drop any resource
records from the additional section. In theory, this has always been a
potential problem, but it is exacerbated by the advent of IPv6. If a host had
several IPv4 addresses and some were not in the additional section, at least
Exim would try the others. However, if a host had both IPv4 and IPv6 addresses
and all the IPv4 (say) addresses were absent, Exim would try only for a IPv6
connection, and never try an IPv4 address. When there was only IPv4
connectivity, this was a disaster that did in practice occur.

So, from release 4.61 onwards, we always search for A and AAAA records
explicitly. The names shouldn't point to CNAMES, but we use the general lookup
function that handles them, just in case. If any lookup gives a soft error,
change the default yield.

For these DNS lookups, we must disable qualify_single and search_parents;
otherwise invalid host names obtained from MX or SRV records can cause trouble
if they happen to match something local. */

yield = HOST_FIND_FAILED;    /* Default yield */
dns_init(FALSE, FALSE,       /* Disable qualify_single and search_parents */
	 dnssec_request || dnssec_require);

for (h = host; h != last->next; h = h->next)
  {
  if (h->address) continue;  /* Inserted by a multihomed host */

  rc = set_address_from_dns(h, &last, ignore_target_hosts, allow_mx_to_ip,
    NULL, dnssec_request, dnssec_require,
    whichrrs & HOST_FIND_IPV4_ONLY
    ?  HOST_FIND_BY_A  :  HOST_FIND_BY_A | HOST_FIND_BY_AAAA);
  if (rc != HOST_FOUND)
    {
    h->status = hstatus_unusable;
    switch (rc)
      {
      case HOST_FIND_AGAIN:	yield = rc; h->why = hwhy_deferred; break;
      case HOST_FIND_SECURITY:	yield = rc; h->why = hwhy_insecure; break;
      case HOST_IGNORED:	h->why = hwhy_ignored; break;
      default:			h->why = hwhy_failed; break;
      }
    }
  }

/* Scan the list for any hosts that are marked unusable because they have
been explicitly ignored, and remove them from the list, as if they did not
exist. If we end up with just a single, ignored host, flatten its fields as if
nothing was found. */

if (ignore_target_hosts)
  {
  host_item *prev = NULL;
  for (h = host; h != last->next; h = h->next)
    {
    REDO:
    if (h->why != hwhy_ignored)        /* Non ignored host, just continue */
      prev = h;
    else if (prev == NULL)             /* First host is ignored */
      {
      if (h != last)                   /* First is not last */
        {
        if (h->next == last) last = h; /* Overwrite it with next */
        *h = *(h->next);               /* and reprocess it. */
        goto REDO;                     /* C should have redo, like Perl */
        }
      }
    else                               /* Ignored host is not first - */
      {                                /*   cut it out */
      prev->next = h->next;
      if (h == last) last = prev;
      }
    }

  if (host->why == hwhy_ignored) host->address = NULL;
  }

/* There is still one complication in the case of IPv6. Although the code above
arranges that IPv6 addresses take precedence over IPv4 addresses for multihomed
hosts, it doesn't do this for addresses that apply to different hosts with the
same MX precedence, because the sorting on MX precedence happens first. So we
have to make another pass to check for this case. We ensure that, within a
single MX preference value, IPv6 addresses come first. This can separate the
addresses of a multihomed host, but that should not matter. */

#if HAVE_IPV6
if (h != last && !disable_ipv6) for (h = host; h != last; h = h->next)
  {
  host_item temp;
  host_item *next = h->next;

  if (  h->mx != next->mx			/* If next is different MX */
     || !h->address				/* OR this one is unset */
     )
    continue;					/* move on to next */

  if (  whichrrs & HOST_FIND_IPV4_FIRST
     ?     !Ustrchr(h->address, ':')		/* OR this one is IPv4 */
        || next->address
           && Ustrchr(next->address, ':')	/* OR next is IPv6 */

     :     Ustrchr(h->address, ':')		/* OR this one is IPv6 */
        || next->address
           && !Ustrchr(next->address, ':')	/* OR next is IPv4 */
     )
    continue;                                /* move on to next */

  temp = *h;                                 /* otherwise, swap */
  temp.next = next->next;
  *h = *next;
  h->next = next;
  *next = temp;
  }
#endif

/* Remove any duplicate IP addresses and then scan the list of hosts for any
whose IP addresses are on the local host. If any are found, all hosts with the
same or higher MX values are removed. However, if the local host has the lowest
numbered MX, then HOST_FOUND_LOCAL is returned. Otherwise, if at least one host
with an IP address is on the list, HOST_FOUND is returned. Otherwise,
HOST_FIND_FAILED is returned, but in this case do not update the yield, as it
might have been set to HOST_FIND_AGAIN just above here. If not, it will already
be HOST_FIND_FAILED. */

host_remove_duplicates(host, &last);
rc = host_scan_for_local_hosts(host, &last, removed);
if (rc != HOST_FIND_FAILED) yield = rc;

DEBUG(D_host_lookup)
  {
  if (fully_qualified_name)
    debug_printf("fully qualified name = %s\n", *fully_qualified_name);
  debug_printf("host_find_bydns yield = %s (%d); returned hosts:\n",
    yield == HOST_FOUND		? "HOST_FOUND" :
    yield == HOST_FOUND_LOCAL	? "HOST_FOUND_LOCAL" :
    yield == HOST_FIND_SECURITY	? "HOST_FIND_SECURITY" :
    yield == HOST_FIND_AGAIN	? "HOST_FIND_AGAIN" :
    yield == HOST_FIND_FAILED	? "HOST_FIND_FAILED" : "?",
    yield);
  for (h = host; h != last->next; h = h->next)
    {
    debug_printf("  %s %s MX=%d %s", h->name,
      !h->address ? US"<null>" : h->address, h->mx,
      h->dnssec == DS_YES ? US"DNSSEC " : US"");
    if (h->port != PORT_NONE) debug_printf("port=%d ", h->port);
    if (h->status >= hstatus_unusable) debug_printf("*");
    debug_printf("\n");
    }
  }

out:

dns_init(FALSE, FALSE, FALSE);	/* clear the dnssec bit for getaddrbyname */
store_free_dns_answer(dnsa);
return yield;
}