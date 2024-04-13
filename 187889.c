get_pubkey_fast (PKT_public_key * pk, u32 * keyid)
{
  int rc = 0;
  KEYDB_HANDLE hd;
  KBNODE keyblock;
  u32 pkid[2];

  assert (pk);
#if MAX_PK_CACHE_ENTRIES
  {
    /* Try to get it from the cache */
    pk_cache_entry_t ce;

    for (ce = pk_cache; ce; ce = ce->next)
      {
	if (ce->keyid[0] == keyid[0] && ce->keyid[1] == keyid[1])
	  {
	    if (pk)
	      copy_public_key (pk, ce->pk);
	    return 0;
	  }
      }
  }
#endif

  hd = keydb_new ();
  rc = keydb_search_kid (hd, keyid);
  if (gpg_err_code (rc) == GPG_ERR_NOT_FOUND)
    {
      keydb_release (hd);
      return G10ERR_NO_PUBKEY;
    }
  rc = keydb_get_keyblock (hd, &keyblock);
  keydb_release (hd);
  if (rc)
    {
      log_error ("keydb_get_keyblock failed: %s\n", g10_errstr (rc));
      return G10ERR_NO_PUBKEY;
    }

  assert (keyblock->pkt->pkttype == PKT_PUBLIC_KEY
	  || keyblock->pkt->pkttype == PKT_PUBLIC_SUBKEY);

  keyid_from_pk (keyblock->pkt->pkt.public_key, pkid);
  if (keyid[0] == pkid[0] && keyid[1] == pkid[1])
    copy_public_key (pk, keyblock->pkt->pkt.public_key);
  else
    rc = G10ERR_NO_PUBKEY;

  release_kbnode (keyblock);

  /* Not caching key here since it won't have all of the fields
     properly set. */

  return rc;
}