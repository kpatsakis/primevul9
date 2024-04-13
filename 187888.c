get_pubkey_byname (ctrl_t ctrl, GETKEY_CTX * retctx, PKT_public_key * pk,
		   const char *name, KBNODE * ret_keyblock,
		   KEYDB_HANDLE * ret_kdbhd, int include_unusable, int no_akl)
{
  int rc;
  strlist_t namelist = NULL;
  struct akl *akl;
  int is_mbox;
  int nodefault = 0;
  int anylocalfirst = 0;

  if (retctx)
    *retctx = NULL;

  is_mbox = is_valid_mailbox (name);

  /* Check whether we the default local search has been disabled.
     This is the case if either the "nodefault" or the "local" keyword
     are in the list of auto key locate mechanisms.

     ANYLOCALFIRST is set if the search order has the local method
     before any other or if "local" is used first by default.  This
     makes sure that if a RETCTX is used it gets only set if a local
     search has precedence over the other search methods and only then
     a followup call to get_pubkey_next shall succeed.  */
  if (!no_akl)
    {
      for (akl = opt.auto_key_locate; akl; akl = akl->next)
	if (akl->type == AKL_NODEFAULT || akl->type == AKL_LOCAL)
	  {
	    nodefault = 1;
	    break;
	  }
      for (akl = opt.auto_key_locate; akl; akl = akl->next)
	if (akl->type != AKL_NODEFAULT)
	  {
	    if (akl->type == AKL_LOCAL)
	      anylocalfirst = 1;
	    break;
	  }
    }

  if (!nodefault)
    anylocalfirst = 1;

  if (nodefault && is_mbox)
    {
      /* Nodefault but a mailbox - let the AKL locate the key.  */
      rc = G10ERR_NO_PUBKEY;
    }
  else
    {
      add_to_strlist (&namelist, name);
      rc = key_byname (retctx, namelist, pk, 0,
		       include_unusable, ret_keyblock, ret_kdbhd);
    }

  /* If the requested name resembles a valid mailbox and automatic
     retrieval has been enabled, we try to import the key. */
  if (gpg_err_code (rc) == G10ERR_NO_PUBKEY && !no_akl && is_mbox)
    {
      for (akl = opt.auto_key_locate; akl; akl = akl->next)
	{
	  unsigned char *fpr = NULL;
	  size_t fpr_len;
	  int did_key_byname = 0;
	  int no_fingerprint = 0;
	  const char *mechanism = "?";

	  switch (akl->type)
	    {
	    case AKL_NODEFAULT:
	      /* This is a dummy mechanism.  */
	      mechanism = "None";
	      rc = G10ERR_NO_PUBKEY;
	      break;

	    case AKL_LOCAL:
	      mechanism = "Local";
	      did_key_byname = 1;
	      if (retctx)
		{
		  get_pubkey_end (*retctx);
		  *retctx = NULL;
		}
	      add_to_strlist (&namelist, name);
	      rc = key_byname (anylocalfirst ? retctx : NULL,
			       namelist, pk, 0,
			       include_unusable, ret_keyblock, ret_kdbhd);
	      break;

	    case AKL_CERT:
	      mechanism = "DNS CERT";
	      glo_ctrl.in_auto_key_retrieve++;
	      rc = keyserver_import_cert (ctrl, name, &fpr, &fpr_len);
	      glo_ctrl.in_auto_key_retrieve--;
	      break;

	    case AKL_PKA:
	      mechanism = "PKA";
	      glo_ctrl.in_auto_key_retrieve++;
	      rc = keyserver_import_pka (ctrl, name, &fpr, &fpr_len);
	      glo_ctrl.in_auto_key_retrieve--;
	      break;

	    case AKL_LDAP:
	      mechanism = "LDAP";
	      glo_ctrl.in_auto_key_retrieve++;
	      rc = keyserver_import_ldap (ctrl, name, &fpr, &fpr_len);
	      glo_ctrl.in_auto_key_retrieve--;
	      break;

	    case AKL_KEYSERVER:
	      /* Strictly speaking, we don't need to only use a valid
	         mailbox for the getname search, but it helps cut down
	         on the problem of searching for something like "john"
	         and getting a whole lot of keys back. */
	      if (opt.keyserver)
		{
		  mechanism = opt.keyserver->uri;
		  glo_ctrl.in_auto_key_retrieve++;
		  rc = keyserver_import_name (ctrl, name, &fpr, &fpr_len,
                                              opt.keyserver);
		  glo_ctrl.in_auto_key_retrieve--;
		}
	      else
		{
		  mechanism = "Unconfigured keyserver";
		  rc = G10ERR_NO_PUBKEY;
		}
	      break;

	    case AKL_SPEC:
	      {
		struct keyserver_spec *keyserver;

		mechanism = akl->spec->uri;
		keyserver = keyserver_match (akl->spec);
		glo_ctrl.in_auto_key_retrieve++;
		rc = keyserver_import_name (ctrl,
                                            name, &fpr, &fpr_len, keyserver);
		glo_ctrl.in_auto_key_retrieve--;
	      }
	      break;
	    }

	  /* Use the fingerprint of the key that we actually fetched.
	     This helps prevent problems where the key that we fetched
	     doesn't have the same name that we used to fetch it.  In
	     the case of CERT and PKA, this is an actual security
	     requirement as the URL might point to a key put in by an
	     attacker.  By forcing the use of the fingerprint, we
	     won't use the attacker's key here. */
	  if (!rc && fpr)
	    {
	      char fpr_string[MAX_FINGERPRINT_LEN * 2 + 1];

	      assert (fpr_len <= MAX_FINGERPRINT_LEN);

	      free_strlist (namelist);
	      namelist = NULL;

	      bin2hex (fpr, fpr_len, fpr_string);

	      if (opt.verbose)
		log_info ("auto-key-locate found fingerprint %s\n",
			  fpr_string);

	      add_to_strlist (&namelist, fpr_string);
	    }
	  else if (!rc && !fpr && !did_key_byname)
	    {
	      no_fingerprint = 1;
	      rc = G10ERR_NO_PUBKEY;
	    }
	  xfree (fpr);
	  fpr = NULL;

	  if (!rc && !did_key_byname)
	    {
	      if (retctx)
		{
		  get_pubkey_end (*retctx);
		  *retctx = NULL;
		}
	      rc = key_byname (anylocalfirst ? retctx : NULL,
			       namelist, pk, 0,
			       include_unusable, ret_keyblock, ret_kdbhd);
	    }
	  if (!rc)
	    {
	      /* Key found.  */
	      log_info (_("automatically retrieved '%s' via %s\n"),
			name, mechanism);
	      break;
	    }
	  if (rc != G10ERR_NO_PUBKEY || opt.verbose || no_fingerprint)
	    log_info (_("error retrieving '%s' via %s: %s\n"),
		      name, mechanism,
		      no_fingerprint ? _("No fingerprint") : g10_errstr (rc));
	}
    }


  if (rc && retctx)
    {
      get_pubkey_end (*retctx);
      *retctx = NULL;
    }

  if (retctx && *retctx)
    {
      assert (!(*retctx)->extra_list);
      (*retctx)->extra_list = namelist;
    }
  else
    free_strlist (namelist);
  return rc;
}