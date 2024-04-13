fascist_firewall_allows_address_or(const tor_addr_t *addr, uint16_t port)
{
  return addr_policy_permits_tor_addr(addr, port,
                                     reachable_or_addr_policy);
}