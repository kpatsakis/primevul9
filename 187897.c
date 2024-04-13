get_pubkey_end (GETKEY_CTX ctx)
{
  if (ctx)
    {
      memset (&ctx->kbpos, 0, sizeof ctx->kbpos);
      keydb_release (ctx->kr_handle);
      free_strlist (ctx->extra_list);
      if (!ctx->not_allocated)
	xfree (ctx);
    }
}