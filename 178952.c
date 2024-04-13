addr_policy_permits_tor_addr(const tor_addr_t *addr, uint16_t port,
                            smartlist_t *policy)
{
  addr_policy_result_t p;
  p = compare_tor_addr_to_addr_policy(addr, port, policy);
  switch (p) {
    case ADDR_POLICY_PROBABLY_ACCEPTED:
    case ADDR_POLICY_ACCEPTED:
      return 1;
    case ADDR_POLICY_PROBABLY_REJECTED:
    case ADDR_POLICY_REJECTED:
      return 0;
    default:
      log_warn(LD_BUG, "Unexpected result: %d", (int)p);
      return 0;
  }
}