cache_user_id (KBNODE keyblock)
{
  user_id_db_t r;
  const char *uid;
  size_t uidlen;
  keyid_list_t keyids = NULL;
  KBNODE k;

  for (k = keyblock; k; k = k->next)
    {
      if (k->pkt->pkttype == PKT_PUBLIC_KEY
	  || k->pkt->pkttype == PKT_PUBLIC_SUBKEY)
	{
	  keyid_list_t a = xmalloc_clear (sizeof *a);
	  /* Hmmm: For a long list of keyids it might be an advantage
	   * to append the keys.  */
	  keyid_from_pk (k->pkt->pkt.public_key, a->keyid);
	  /* First check for duplicates.  */
	  for (r = user_id_db; r; r = r->next)
	    {
	      keyid_list_t b = r->keyids;
	      for (b = r->keyids; b; b = b->next)
		{
		  if (b->keyid[0] == a->keyid[0]
		      && b->keyid[1] == a->keyid[1])
		    {
		      if (DBG_CACHE)
			log_debug ("cache_user_id: already in cache\n");
		      release_keyid_list (keyids);
		      xfree (a);
		      return;
		    }
		}
	    }
	  /* Now put it into the cache.  */
	  a->next = keyids;
	  keyids = a;
	}
    }
  if (!keyids)
    BUG (); /* No key no fun.  */


  uid = get_primary_uid (keyblock, &uidlen);

  if (uid_cache_entries >= MAX_UID_CACHE_ENTRIES)
    {
      /* fixme: use another algorithm to free some cache slots */
      r = user_id_db;
      user_id_db = r->next;
      release_keyid_list (r->keyids);
      xfree (r);
      uid_cache_entries--;
    }
  r = xmalloc (sizeof *r + uidlen - 1);
  r->keyids = keyids;
  r->len = uidlen;
  memcpy (r->name, uid, r->len);
  r->next = user_id_db;
  user_id_db = r;
  uid_cache_entries++;
}