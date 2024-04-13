parse_auto_key_locate (char *options)
{
  char *tok;

  while ((tok = optsep (&options)))
    {
      struct akl *akl, *check, *last = NULL;
      int dupe = 0;

      if (tok[0] == '\0')
	continue;

      akl = xmalloc_clear (sizeof (*akl));

      if (ascii_strcasecmp (tok, "nodefault") == 0)
	akl->type = AKL_NODEFAULT;
      else if (ascii_strcasecmp (tok, "local") == 0)
	akl->type = AKL_LOCAL;
      else if (ascii_strcasecmp (tok, "ldap") == 0)
	akl->type = AKL_LDAP;
      else if (ascii_strcasecmp (tok, "keyserver") == 0)
	akl->type = AKL_KEYSERVER;
#ifdef USE_DNS_CERT
      else if (ascii_strcasecmp (tok, "cert") == 0)
	akl->type = AKL_CERT;
#endif
#ifdef USE_DNS_PKA
      else if (ascii_strcasecmp (tok, "pka") == 0)
	akl->type = AKL_PKA;
#endif
      else if ((akl->spec = parse_keyserver_uri (tok, 1, NULL, 0)))
	akl->type = AKL_SPEC;
      else
	{
	  free_akl (akl);
	  return 0;
	}

      /* We must maintain the order the user gave us */
      for (check = opt.auto_key_locate; check;
	   last = check, check = check->next)
	{
	  /* Check for duplicates */
	  if (check->type == akl->type
	      && (akl->type != AKL_SPEC
		  || (akl->type == AKL_SPEC
		      && strcmp (check->spec->uri, akl->spec->uri) == 0)))
	    {
	      dupe = 1;
	      free_akl (akl);
	      break;
	    }
	}

      if (!dupe)
	{
	  if (last)
	    last->next = akl;
	  else
	    opt.auto_key_locate = akl;
	}
    }

  return 1;
}