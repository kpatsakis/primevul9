compare_tor_addr_to_addr_policy(const tor_addr_t *addr, uint16_t port,
                                const smartlist_t *policy)
{
  if (!policy) {
    /* no policy? accept all. */
    return ADDR_POLICY_ACCEPTED;
  } else if (tor_addr_is_null(addr)) {
    if (port == 0) {
      log_info(LD_BUG, "Rejecting null address with 0 port (family %d)",
               addr ? tor_addr_family(addr) : -1);
      return ADDR_POLICY_REJECTED;
    }
    return compare_unknown_tor_addr_to_addr_policy(port, policy);
  } else if (port == 0) {
    return compare_known_tor_addr_to_addr_policy_noport(addr, policy);
  } else {
    return compare_known_tor_addr_to_addr_policy(addr, port, policy);
  }
}