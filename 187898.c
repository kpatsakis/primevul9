get_long_user_id_string (u32 * keyid)
{
  user_id_db_t r;
  char *p;
  int pass = 0;
  /* Try it two times; second pass reads from key resources.  */
  do
    {
      for (r = user_id_db; r; r = r->next)
	{
	  keyid_list_t a;
	  for (a = r->keyids; a; a = a->next)
	    {
	      if (a->keyid[0] == keyid[0] && a->keyid[1] == keyid[1])
		{
		  p = xmalloc (r->len + 20);
		  sprintf (p, "%08lX%08lX %.*s",
			   (ulong) keyid[0], (ulong) keyid[1],
			   r->len, r->name);
		  return p;
		}
	    }
	}
    }
  while (++pass < 2 && !get_pubkey (NULL, keyid));
  p = xmalloc (25);
  sprintf (p, "%08lX%08lX [?]", (ulong) keyid[0], (ulong) keyid[1]);
  return p;
}