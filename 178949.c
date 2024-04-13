addr_policy_permits_address(uint32_t addr, uint16_t port,
                            smartlist_t *policy)
{
  tor_addr_t a;
  tor_addr_from_ipv4h(&a, addr);
  return addr_policy_permits_tor_addr(&a, port, policy);
}