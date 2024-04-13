authdir_policy_baddir_address(uint32_t addr, uint16_t port)
{
  return ! addr_policy_permits_address(addr, port, authdir_baddir_policy);
}