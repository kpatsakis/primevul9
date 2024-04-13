get_user_id (u32 * keyid, size_t * rn)
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
		  p = xmalloc (r->len);
		  memcpy (p, r->name, r->len);
		  *rn = r->len;
		  return p;
		}
	    }
	}
    }
  while (++pass < 2 && !get_pubkey (NULL, keyid));
  p = xstrdup (user_id_not_found_utf8 ());
  *rn = strlen (p);
  return p;
}