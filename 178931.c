authdir_policy_valid_address(uint32_t addr, uint16_t port)
{
  return addr_policy_permits_address(addr, port, authdir_invalid_policy);
}