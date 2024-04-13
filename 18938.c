router_parse_addr_policy_item_from_string(const char *s, int assume_action)
{
  directory_token_t *tok = NULL;
  const char *cp, *eos;
  /* Longest possible policy is "accept ffff:ffff:..255/ffff:...255:0-65535".
   * But note that there can be an arbitrary amount of space between the
   * accept and the address:mask/port element. */
  char line[TOR_ADDR_BUF_LEN*2 + 32];
  addr_policy_t *r;
  memarea_t *area = NULL;

  s = eat_whitespace(s);
  if ((*s == '*' || TOR_ISDIGIT(*s)) && assume_action >= 0) {
    if (tor_snprintf(line, sizeof(line), "%s %s",
               assume_action == ADDR_POLICY_ACCEPT?"accept":"reject", s)<0) {
      log_warn(LD_DIR, "Policy %s is too long.", escaped(s));
      return NULL;
    }
    cp = line;
    tor_strlower(line);
  } else { /* assume an already well-formed address policy line */
    cp = s;
  }

  eos = cp + strlen(cp);
  area = memarea_new();
  tok = get_next_token(area, &cp, eos, routerdesc_token_table);
  if (tok->tp == _ERR) {
    log_warn(LD_DIR, "Error reading address policy: %s", tok->error);
    goto err;
  }
  if (tok->tp != K_ACCEPT && tok->tp != K_ACCEPT6 &&
      tok->tp != K_REJECT && tok->tp != K_REJECT6) {
    log_warn(LD_DIR, "Expected 'accept' or 'reject'.");
    goto err;
  }

  r = router_parse_addr_policy(tok);
  goto done;
 err:
  r = NULL;
 done:
  token_clear(tok);
  if (area) {
    DUMP_AREA(area, "policy item");
    memarea_drop_all(area);
  }
  return r;
}