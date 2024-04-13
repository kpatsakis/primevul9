get_pubkey_byfprint_fast (PKT_public_key * pk,
			  const byte * fprint, size_t fprint_len)
{
  int rc = 0;
  KEYDB_HANDLE hd;
  KBNODE keyblock;
  byte fprbuf[MAX_FINGERPRINT_LEN];
  int i;

  for (i = 0; i < MAX_FINGERPRINT_LEN && i < fprint_len; i++)
    fprbuf[i] = fprint[i];
  while (i < MAX_FINGERPRINT_LEN)
    fprbuf[i++] = 0;

  hd = keydb_new ();
  rc = keydb_search_fpr (hd, fprbuf);
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
  if (pk)
    copy_public_key (pk, keyblock->pkt->pkt.public_key);
  release_kbnode (keyblock);

  /* Not caching key here since it won't have all of the fields
     properly set. */

  return 0;
}