update_reader_status_file (int set_card_removed_flag)
{
  int idx;
  unsigned int status, changed;

  /* Note, that we only try to get the status, because it does not
     make sense to wait here for a operation to complete.  If we are
     busy working with a card, delays in the status file update should
     be acceptable. */
  for (idx=0; idx < DIM(vreader_table); idx++)
    {
      struct vreader_s *vr = vreader_table + idx;
      struct server_local_s *sl;
      int sw_apdu;

      if (!vr->valid || vr->slot == -1)
        continue; /* Not valid or reader not yet open. */

      sw_apdu = apdu_get_status (vr->slot, 0, &status, &changed);
      if (sw_apdu == SW_HOST_NO_READER)
        {
          /* Most likely the _reader_ has been unplugged.  */
          application_notify_card_reset (vr->slot);
	  apdu_close_reader (vr->slot);
          vr->slot = -1;
          status = 0;
          changed = vr->changed;
        }
      else if (sw_apdu)
        {
          /* Get status failed.  Ignore that.  */
          continue;
        }

      if (!vr->any || vr->status != status || vr->changed != changed )
        {
          char *fname;
          char templ[50];
          FILE *fp;

          log_info ("updating reader %d (%d) status: 0x%04X->0x%04X (%u->%u)\n",
                    idx, vr->slot, vr->status, status, vr->changed, changed);
          vr->status = status;
          vr->changed = changed;

	  /* FIXME: Should this be IDX instead of vr->slot?  This
	     depends on how client sessions will associate the reader
	     status with their session.  */
          snprintf (templ, sizeof templ, "reader_%d.status", vr->slot);
          fname = make_filename (opt.homedir, templ, NULL );
          fp = fopen (fname, "w");
          if (fp)
            {
              fprintf (fp, "%s\n",
                       (status & 1)? "USABLE":
                       (status & 4)? "ACTIVE":
                       (status & 2)? "PRESENT": "NOCARD");
              fclose (fp);
            }
          xfree (fname);

          /* If a status script is executable, run it. */
          {
            const char *args[9], *envs[2];
            char numbuf1[30], numbuf2[30], numbuf3[30];
            char *homestr, *envstr;
            gpg_error_t err;

            homestr = make_filename (opt.homedir, NULL);
            if (gpgrt_asprintf (&envstr, "GNUPGHOME=%s", homestr) < 0)
              log_error ("out of core while building environment\n");
            else
              {
                envs[0] = envstr;
                envs[1] = NULL;

                sprintf (numbuf1, "%d", vr->slot);
                sprintf (numbuf2, "0x%04X", vr->status);
                sprintf (numbuf3, "0x%04X", status);
                args[0] = "--reader-port";
                args[1] = numbuf1;
                args[2] = "--old-code";
                args[3] = numbuf2;
                args[4] = "--new-code";
                args[5] = numbuf3;
                args[6] = "--status";
                args[7] = ((status & 1)? "USABLE":
                           (status & 4)? "ACTIVE":
                           (status & 2)? "PRESENT": "NOCARD");
                args[8] = NULL;

                fname = make_filename (opt.homedir, "scd-event", NULL);
                err = gnupg_spawn_process_detached (fname, args, envs);
                if (err && gpg_err_code (err) != GPG_ERR_ENOENT)
                  log_error ("failed to run event handler '%s': %s\n",
                             fname, gpg_strerror (err));
                xfree (fname);
                xfree (envstr);
              }
            xfree (homestr);
          }

          /* Set the card removed flag for all current sessions.  */
          if (vr->any && vr->status == 0 && set_card_removed_flag)
            update_card_removed (idx, 1);

          vr->any = 1;

          /* Send a signal to all clients who applied for it.  */
          send_client_notifications ();
        }

      /* Check whether a disconnect is pending.  */
      if (opt.card_timeout)
        {
          for (sl=session_list; sl; sl = sl->next_session)
            if (!sl->disconnect_allowed)
              break;
          if (session_list && !sl)
            {
              /* FIXME: Use a real timeout.  */
              /* At least one connection and all allow a disconnect.  */
              log_info ("disconnecting card in reader %d (%d)\n",
                        idx, vr->slot);
              apdu_disconnect (vr->slot);
            }
        }

    }
}