router_parse_addr_policy(directory_token_t *tok)
{
  addr_policy_t newe;
  char *arg;

  tor_assert(tok->tp == K_REJECT || tok->tp == K_REJECT6 ||
             tok->tp == K_ACCEPT || tok->tp == K_ACCEPT6);

  if (tok->n_args != 1)
    return NULL;
  arg = tok->args[0];

  if (!strcmpstart(arg,"private"))
    return router_parse_addr_policy_private(tok);

  memset(&newe, 0, sizeof(newe));

  if (tok->tp == K_REJECT || tok->tp == K_REJECT6)
    newe.policy_type = ADDR_POLICY_REJECT;
  else
    newe.policy_type = ADDR_POLICY_ACCEPT;

  if (tor_addr_parse_mask_ports(arg, &newe.addr, &newe.maskbits,
                                &newe.prt_min, &newe.prt_max) < 0) {
    log_warn(LD_DIR,"Couldn't parse line %s. Dropping", escaped(arg));
    return NULL;
  }

  return addr_policy_get_canonical_entry(&newe);
}