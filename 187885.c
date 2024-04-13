get_keyblock_byfprint (KBNODE * ret_keyblock, const byte * fprint,
		       size_t fprint_len)
{
  int rc;

  if (fprint_len == 20 || fprint_len == 16)
    {
      struct getkey_ctx_s ctx;

      memset (&ctx, 0, sizeof ctx);
      ctx.not_allocated = 1;
      ctx.kr_handle = keydb_new ();
      ctx.nitems = 1;
      ctx.items[0].mode = (fprint_len == 16
                           ? KEYDB_SEARCH_MODE_FPR16
                           : KEYDB_SEARCH_MODE_FPR20);
      memcpy (ctx.items[0].u.fpr, fprint, fprint_len);
      rc = lookup (&ctx, ret_keyblock, 0);
      get_pubkey_end (&ctx);
    }
  else
    rc = G10ERR_GENERAL; /* Oops */

  return rc;
}