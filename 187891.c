get_pubkey (PKT_public_key * pk, u32 * keyid)
{
  int internal = 0;
  int rc = 0;

#if MAX_PK_CACHE_ENTRIES
  if (pk)
    {
      /* Try to get it from the cache.  We don't do this when pk is
         NULL as it does not guarantee that the user IDs are
         cached. */
      pk_cache_entry_t ce;
      for (ce = pk_cache; ce; ce = ce->next)
	{
	  if (ce->keyid[0] == keyid[0] && ce->keyid[1] == keyid[1])
	    {
	      copy_public_key (pk, ce->pk);
	      return 0;
	    }
	}
    }
#endif
  /* More init stuff.  */
  if (!pk)
    {
      pk = xmalloc_clear (sizeof *pk);
      internal++;
    }


  /* Do a lookup.  */
  {
    struct getkey_ctx_s ctx;
    KBNODE kb = NULL;
    memset (&ctx, 0, sizeof ctx);
    ctx.exact = 1; /* Use the key ID exactly as given.  */
    ctx.not_allocated = 1;
    ctx.kr_handle = keydb_new ();
    ctx.nitems = 1;
    ctx.items[0].mode = KEYDB_SEARCH_MODE_LONG_KID;
    ctx.items[0].u.kid[0] = keyid[0];
    ctx.items[0].u.kid[1] = keyid[1];
    ctx.req_algo = pk->req_algo;
    ctx.req_usage = pk->req_usage;
    rc = lookup (&ctx, &kb, 0);
    if (!rc)
      {
	pk_from_block (&ctx, pk, kb);
      }
    get_pubkey_end (&ctx);
    release_kbnode (kb);
  }
  if (!rc)
    goto leave;

  rc = G10ERR_NO_PUBKEY;

leave:
  if (!rc)
    cache_public_key (pk);
  if (internal)
    free_public_key (pk);
  return rc;
}