dir_policy_permits_address(const tor_addr_t *addr)
{
  return addr_policy_permits_tor_addr(addr, 1, dir_policy);
}