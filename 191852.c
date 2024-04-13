add_fingerprint_to_dir(const char *fp, authdir_config_t *list,
                       router_status_t add_status)
{
  char *fingerprint;
  char d[DIGEST_LEN];
  router_status_t *status;
  tor_assert(fp);
  tor_assert(list);

  fingerprint = tor_strdup(fp);
  tor_strstrip(fingerprint, " ");
  if (base16_decode(d, DIGEST_LEN,
                    fingerprint, strlen(fingerprint)) != DIGEST_LEN) {
    log_warn(LD_DIRSERV, "Couldn't decode fingerprint \"%s\"",
             escaped(fp));
    tor_free(fingerprint);
    return;
  }

  status = digestmap_get(list->status_by_digest, d);
  if (!status) {
    status = tor_malloc_zero(sizeof(router_status_t));
    digestmap_set(list->status_by_digest, d, status);
  }

  tor_free(fingerprint);
  *status |= add_status;
  return;
}