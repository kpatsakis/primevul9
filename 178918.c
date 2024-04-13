compare_addr_to_addr_policy(uint32_t addr, uint16_t port,
                            const smartlist_t *policy)
{
  /*XXXX deprecate this function when possible. */
  tor_addr_t a;
  tor_addr_from_ipv4h(&a, addr);
  return compare_tor_addr_to_addr_policy(&a, port, policy);
}