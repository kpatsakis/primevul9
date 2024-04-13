have_secret_key_with_kid (u32 *keyid)
{
  gpg_error_t err;
  KEYDB_HANDLE kdbhd;
  KEYDB_SEARCH_DESC desc;
  kbnode_t keyblock;
  kbnode_t node;
  int result = 0;

  kdbhd = keydb_new ();
  memset (&desc, 0, sizeof desc);
  desc.mode = KEYDB_SEARCH_MODE_LONG_KID;
  desc.u.kid[0] = keyid[0];
  desc.u.kid[1] = keyid[1];
  while (!result && !(err = keydb_search (kdbhd, &desc, 1)))
    {
      desc.mode = KEYDB_SEARCH_MODE_NEXT;
      err = keydb_get_keyblock (kdbhd, &keyblock);
      if (err)
        {
          log_error (_("error reading keyblock: %s\n"), g10_errstr (err));
          break;
        }

      for (node = keyblock; node; node = node->next)
	{
          /* Bit 0 of the flags is set if the search found the key
             using that key or subkey.  */
	  if ((node->flag & 1))
            {
              assert (node->pkt->pkttype == PKT_PUBLIC_KEY
                      || node->pkt->pkttype == PKT_PUBLIC_SUBKEY);

              if (!agent_probe_secret_key (NULL, node->pkt->pkt.public_key))
                {
                  result = 1;
                  break;
                }
	    }
	}
      release_kbnode (keyblock);
    }
  keydb_release (kdbhd);
  return result;
}