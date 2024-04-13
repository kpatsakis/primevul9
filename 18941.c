find_dir_signing_key(const char *str, const char *eos)
{
  const char *cp;
  directory_token_t *tok;
  crypto_pk_env_t *key = NULL;
  memarea_t *area = NULL;
  tor_assert(str);
  tor_assert(eos);

  /* Is there a dir-signing-key in the directory? */
  cp = tor_memstr(str, eos-str, "\nopt dir-signing-key");
  if (!cp)
    cp = tor_memstr(str, eos-str, "\ndir-signing-key");
  if (!cp)
    return NULL;
  ++cp; /* Now cp points to the start of the token. */

  area = memarea_new();
  tok = get_next_token(area, &cp, eos, dir_token_table);
  if (!tok) {
    log_warn(LD_DIR, "Unparseable dir-signing-key token");
    goto done;
  }
  if (tok->tp != K_DIR_SIGNING_KEY) {
    log_warn(LD_DIR, "Dir-signing-key token did not parse as expected");
    goto done;
  }

  if (tok->key) {
    key = tok->key;
    tok->key = NULL; /* steal reference. */
  } else {
    log_warn(LD_DIR, "Dir-signing-key token contained no key");
  }

 done:
  if (tok) token_clear(tok);
  if (area) {
    DUMP_AREA(area, "dir-signing-key token");
    memarea_drop_all(area);
  }
  return key;
}