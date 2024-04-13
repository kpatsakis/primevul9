host_fake_gethostbyname(const uschar *name, int af, int *error_num)
{
#if HAVE_IPV6
int alen = (af == AF_INET)? sizeof(struct in_addr):sizeof(struct in6_addr);
#else
int alen = sizeof(struct in_addr);
#endif

int ipa;
const uschar *lname = name;
uschar *adds;
uschar **alist;
struct hostent *yield;
dns_answer * dnsa = store_get_dns_answer();
dns_scan dnss;

DEBUG(D_host_lookup)
  debug_printf("using host_fake_gethostbyname for %s (%s)\n", name,
    af == AF_INET ? "IPv4" : "IPv6");

/* Handle unqualified "localhost" */

if (Ustrcmp(name, "localhost") == 0)
  lname = af == AF_INET ? US"127.0.0.1" : US"::1";

/* Handle a literal IP address */

if ((ipa = string_is_ip_address(lname, NULL)) != 0)
  if (   ipa == 4 && af == AF_INET
     ||  ipa == 6 && af == AF_INET6)
    {
    int x[4];
    yield = store_get(sizeof(struct hostent), FALSE);
    alist = store_get(2 * sizeof(char *), FALSE);
    adds  = store_get(alen, FALSE);
    yield->h_name = CS name;
    yield->h_aliases = NULL;
    yield->h_addrtype = af;
    yield->h_length = alen;
    yield->h_addr_list = CSS alist;
    *alist++ = adds;
    for (int n = host_aton(lname, x), i = 0; i < n; i++)
      {
      int y = x[i];
      *adds++ = (y >> 24) & 255;
      *adds++ = (y >> 16) & 255;
      *adds++ = (y >> 8) & 255;
      *adds++ = y & 255;
      }
    *alist = NULL;
    }

  /* Wrong kind of literal address */

  else
    {
    *error_num = HOST_NOT_FOUND;
    yield = NULL;
    goto out;
    }

/* Handle a host name */

else
  {
  int type = af == AF_INET ? T_A:T_AAAA;
  int rc = dns_lookup_timerwrap(dnsa, lname, type, NULL);
  int count = 0;

  lookup_dnssec_authenticated = NULL;

  switch(rc)
    {
    case DNS_SUCCEED: break;
    case DNS_NOMATCH: *error_num = HOST_NOT_FOUND; yield = NULL; goto out;
    case DNS_NODATA:  *error_num = NO_DATA; yield = NULL; goto out;
    case DNS_AGAIN:   *error_num = TRY_AGAIN; yield = NULL; goto out;
    default:
    case DNS_FAIL:    *error_num = NO_RECOVERY; yield = NULL; goto out;
    }

  for (dns_record * rr = dns_next_rr(dnsa, &dnss, RESET_ANSWERS);
       rr;
       rr = dns_next_rr(dnsa, &dnss, RESET_NEXT)) if (rr->type == type)
    count++;

  yield = store_get(sizeof(struct hostent), FALSE);
  alist = store_get((count + 1) * sizeof(char *), FALSE);
  adds  = store_get(count *alen, FALSE);

  yield->h_name = CS name;
  yield->h_aliases = NULL;
  yield->h_addrtype = af;
  yield->h_length = alen;
  yield->h_addr_list = CSS alist;

  for (dns_record * rr = dns_next_rr(dnsa, &dnss, RESET_ANSWERS);
       rr;
       rr = dns_next_rr(dnsa, &dnss, RESET_NEXT)) if (rr->type == type)
    {
    int x[4];
    dns_address *da;
    if (!(da = dns_address_from_rr(dnsa, rr))) break;
    *alist++ = adds;
    for (int n = host_aton(da->address, x), i = 0; i < n; i++)
      {
      int y = x[i];
      *adds++ = (y >> 24) & 255;
      *adds++ = (y >> 16) & 255;
      *adds++ = (y >> 8) & 255;
      *adds++ = y & 255;
      }
    }
  *alist = NULL;
  }

out:

store_free_dns_answer(dnsa);
return yield;
}