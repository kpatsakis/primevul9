do_reset (ctrl_t ctrl, int send_reset)
{
  int vrdr = ctrl->server_local->vreader_idx;
  int slot;
  int err;

  if (!(vrdr == -1 || (vrdr >= 0 && vrdr < DIM(vreader_table))))
    BUG ();

  /* If there is an active application, release it.  Tell all other
     sessions using the same application to release the
     application.  */
  if (ctrl->app_ctx)
    {
      release_application (ctrl->app_ctx);
      ctrl->app_ctx = NULL;
      if (send_reset)
        {
          struct server_local_s *sl;

          for (sl=session_list; sl; sl = sl->next_session)
            if (sl->ctrl_backlink
                && sl->ctrl_backlink->server_local->vreader_idx == vrdr)
              {
                sl->app_ctx_marked_for_release = 1;
              }
        }
    }

  /* If we want a real reset for the card, send the reset APDU and
     tell the application layer about it.  */
  slot = vreader_slot (vrdr);
  if (slot != -1 && send_reset && !IS_LOCKED (ctrl) )
    {
      application_notify_card_reset (slot);
      switch (apdu_reset (slot))
        {
        case 0:
          break;
        case SW_HOST_NO_CARD:
        case SW_HOST_CARD_INACTIVE:
          break;
        default:
	  apdu_close_reader (slot);
          vreader_table[vrdr].slot = slot = -1;
          break;
        }
    }

  /* If we hold a lock, unlock now. */
  if (locked_session && ctrl->server_local == locked_session)
    {
      locked_session = NULL;
      log_info ("implicitly unlocking due to RESET\n");
    }

  /* Reset the card removed flag for the current reader.  We need to
     take the lock here so that the ticker thread won't concurrently
     try to update the file.  Calling update_reader_status_file is
     required to get hold of the new status of the card in the vreader
     table.  */
  err = npth_mutex_lock (&status_file_update_lock);
  if (err)
    {
      log_error ("failed to acquire status_file_update lock\n");
      ctrl->server_local->vreader_idx = -1;
      return;
    }
  update_reader_status_file (0);  /* Update slot status table.  */
  update_card_removed (vrdr, 0);  /* Clear card_removed flag.  */
  err = npth_mutex_unlock (&status_file_update_lock);
  if (err)
    log_error ("failed to release status_file_update lock: %s\n",
	       strerror (err));

  /* Do this last, so that the update_card_removed above does its job.  */
  ctrl->server_local->vreader_idx = -1;
}