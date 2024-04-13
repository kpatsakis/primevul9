policy_write_item(char *buf, size_t buflen, addr_policy_t *policy,
                  int format_for_desc)
{
  size_t written = 0;
  char addrbuf[TOR_ADDR_BUF_LEN];
  const char *addrpart;
  int result;
  const int is_accept = policy->policy_type == ADDR_POLICY_ACCEPT;
  const int is_ip6 = tor_addr_family(&policy->addr) == AF_INET6;

  tor_addr_to_str(addrbuf, &policy->addr, sizeof(addrbuf), 1);

  /* write accept/reject 1.2.3.4 */
  if (policy->is_private)
    addrpart = "private";
  else if (policy->maskbits == 0)
    addrpart = "*";
  else
    addrpart = addrbuf;

  result = tor_snprintf(buf, buflen, "%s%s%s %s",
                        (is_ip6&&format_for_desc)?"opt ":"",
                        is_accept ? "accept" : "reject",
                        (is_ip6&&format_for_desc)?"6":"",
                        addrpart);
  if (result < 0)
    return -1;
  written += strlen(buf);
  /* If the maskbits is 32 we don't need to give it.  If the mask is 0,
   * we already wrote "*". */
  if (policy->maskbits < 32 && policy->maskbits > 0) {
    if (tor_snprintf(buf+written, buflen-written, "/%d", policy->maskbits)<0)
      return -1;
    written += strlen(buf+written);
  }
  if (policy->prt_min <= 1 && policy->prt_max == 65535) {
    /* There is no port set; write ":*" */
    if (written+4 > buflen)
      return -1;
    strlcat(buf+written, ":*", buflen-written);
    written += 2;
  } else if (policy->prt_min == policy->prt_max) {
    /* There is only one port; write ":80". */
    result = tor_snprintf(buf+written, buflen-written, ":%d", policy->prt_min);
    if (result<0)
      return -1;
    written += result;
  } else {
    /* There is a range of ports; write ":79-80". */
    result = tor_snprintf(buf+written, buflen-written, ":%d-%d",
                          policy->prt_min, policy->prt_max);
    if (result<0)
      return -1;
    written += result;
  }
  if (written < buflen)
    buf[written] = '\0';
  else
    return -1;

  return (int)written;
}