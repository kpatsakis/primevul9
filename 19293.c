open_card (ctrl_t ctrl, const char *apptype)
{
  gpg_error_t err;
  int vrdr;

  /* If we ever got a card not present error code, return that.  Only
     the SERIALNO command and a reset are able to clear from that
     state. */
  if (ctrl->server_local->card_removed)
    return gpg_error (GPG_ERR_CARD_REMOVED);

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  /* If the application has been marked for release do it now.  We
     can't do it immediately in do_reset because the application may
     still be in use.  */
  if (ctrl->server_local->app_ctx_marked_for_release)
    {
      ctrl->server_local->app_ctx_marked_for_release = 0;
      release_application (ctrl->app_ctx);
      ctrl->app_ctx = NULL;
    }

  /* If we are already initialized for one specific application we
     need to check that the client didn't requested a specific
     application different from the one in use before we continue. */
  if (ctrl->app_ctx)
    {
      return check_application_conflict
        (ctrl, vreader_slot (ctrl->server_local->vreader_idx), apptype);
    }

  /* Setup the vreader and select the application.  */
  if (ctrl->server_local->vreader_idx != -1)
    vrdr = ctrl->server_local->vreader_idx;
  else
    vrdr = get_current_reader ();
  ctrl->server_local->vreader_idx = vrdr;
  if (vrdr == -1)
    err = gpg_error (GPG_ERR_CARD);
  else
    {
      /* Fixme: We should move the apdu_connect call to
         select_application.  */
      int sw;
      int slot = vreader_slot (vrdr);

      ctrl->server_local->disconnect_allowed = 0;
      sw = apdu_connect (slot);
      if (sw && sw != SW_HOST_ALREADY_CONNECTED)
        {
          if (sw == SW_HOST_NO_CARD)
            err = gpg_error (GPG_ERR_CARD_NOT_PRESENT);
          else if (sw == SW_HOST_CARD_INACTIVE)
            err = gpg_error (GPG_ERR_CARD_RESET);
          else
            err = gpg_error (GPG_ERR_CARD);
	}
      else
        err = select_application (ctrl, slot, apptype, &ctrl->app_ctx);
    }

  TEST_CARD_REMOVAL (ctrl, err);
  return err;
}