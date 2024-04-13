get_user_id_string (u32 * keyid)
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
		  p = xmalloc (keystrlen () + 1 + r->len + 1);
		  sprintf (p, "%s %.*s", keystr (keyid), r->len, r->name);
		  return p;
		}
	    }
	}
    }
  while (++pass < 2 && !get_pubkey (NULL, keyid));
  p = xmalloc (keystrlen () + 5);
  sprintf (p, "%s [?]", keystr (keyid));
  return p;
}