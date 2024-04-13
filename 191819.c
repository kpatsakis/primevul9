dirserv_router_has_valid_address(routerinfo_t *ri)
{
  tor_addr_t addr;
  if (get_options()->DirAllowPrivateAddresses)
    return 0; /* whatever it is, we're fine with it */
  tor_addr_from_ipv4h(&addr, ri->addr);

  if (tor_addr_is_internal(&addr, 0)) {
    log_info(LD_DIRSERV,
             "Router %s published internal IP address. Refusing.",
             router_describe(ri));
    return -1; /* it's a private IP, we should reject it */
  }
  return 0;
}