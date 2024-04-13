get_pubkey_byfprint (PKT_public_key * pk,
		     const byte * fprint, size_t fprint_len)
{
  int rc;

  if (fprint_len == 20 || fprint_len == 16)
    {
      struct getkey_ctx_s ctx;
      KBNODE kb = NULL;

      memset (&ctx, 0, sizeof ctx);
      ctx.exact = 1;
      ctx.not_allocated = 1;
      ctx.kr_handle = keydb_new ();
      ctx.nitems = 1;
      ctx.items[0].mode = fprint_len == 16 ? KEYDB_SEARCH_MODE_FPR16
	: KEYDB_SEARCH_MODE_FPR20;
      memcpy (ctx.items[0].u.fpr, fprint, fprint_len);
      rc = lookup (&ctx, &kb, 0);
      if (!rc && pk)
	pk_from_block (&ctx, pk, kb);
      release_kbnode (kb);
      get_pubkey_end (&ctx);
    }
  else
    rc = G10ERR_GENERAL; /* Oops */
  return rc;
}