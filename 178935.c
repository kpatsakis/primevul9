fascist_firewall_allows_or(routerinfo_t *ri)
{
  /* XXXX proposal 118 */
  tor_addr_t addr;
  tor_addr_from_ipv4h(&addr, ri->addr);
  return fascist_firewall_allows_address_or(&addr, ri->or_port);
}