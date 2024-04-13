cache_public_key (PKT_public_key * pk)
{
#if MAX_PK_CACHE_ENTRIES
  pk_cache_entry_t ce;
  u32 keyid[2];

  if (pk_cache_disabled)
    return;

  if (pk->flags.dont_cache)
    return;

  if (is_ELGAMAL (pk->pubkey_algo)
      || pk->pubkey_algo == PUBKEY_ALGO_DSA
      || pk->pubkey_algo == PUBKEY_ALGO_ECDSA
      || pk->pubkey_algo == PUBKEY_ALGO_ECDH
      || is_RSA (pk->pubkey_algo))
    {
      keyid_from_pk (pk, keyid);
    }
  else
    return; /* Don't know how to get the keyid.  */

  for (ce = pk_cache; ce; ce = ce->next)
    if (ce->keyid[0] == keyid[0] && ce->keyid[1] == keyid[1])
      {
	if (DBG_CACHE)
	  log_debug ("cache_public_key: already in cache\n");
	return;
      }

  if (pk_cache_entries >= MAX_PK_CACHE_ENTRIES)
    {
      /* fixme: Use another algorithm to free some cache slots.  */
      pk_cache_disabled = 1;
      if (opt.verbose > 1)
	log_info (_("too many entries in pk cache - disabled\n"));
      return;
    }
  pk_cache_entries++;
  ce = xmalloc (sizeof *ce);
  ce->next = pk_cache;
  pk_cache = ce;
  ce->pk = copy_public_key (NULL, pk);
  ce->keyid[0] = keyid[0];
  ce->keyid[1] = keyid[1];
#endif
}