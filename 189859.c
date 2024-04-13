int imap_read_headers (IMAP_DATA* idata, unsigned int msn_begin, unsigned int msn_end)
{
  CONTEXT* ctx;
  char *hdrreq = NULL;
  FILE *fp;
  char tempfile[_POSIX_PATH_MAX];
  int msgno, idx;
  IMAP_HEADER h;
  IMAP_STATUS* status;
  int rc, mfhrc = 0, oldmsgcount;
  int fetch_msn_end = 0;
  unsigned int maxuid = 0;
  static const char * const want_headers = "DATE FROM SUBJECT TO CC MESSAGE-ID REFERENCES CONTENT-TYPE CONTENT-DESCRIPTION IN-REPLY-TO REPLY-TO LINES LIST-POST X-LABEL";
  progress_t progress;
  int retval = -1;
  int evalhc = 0;

#if USE_HCACHE
  char buf[LONG_STRING];
  unsigned int *uid_validity = NULL;
  unsigned int *puidnext = NULL;
  unsigned int uidnext = 0;
#endif /* USE_HCACHE */

  ctx = idata->ctx;

  if (mutt_bit_isset (idata->capabilities,IMAP4REV1))
  {
    safe_asprintf (&hdrreq, "BODY.PEEK[HEADER.FIELDS (%s%s%s)]",
                           want_headers, ImapHeaders ? " " : "", NONULL (ImapHeaders));
  }
  else if (mutt_bit_isset (idata->capabilities,IMAP4))
  {
    safe_asprintf (&hdrreq, "RFC822.HEADER.LINES (%s%s%s)",
                           want_headers, ImapHeaders ? " " : "", NONULL (ImapHeaders));
  }
  else
  {	/* Unable to fetch headers for lower versions */
    mutt_error _("Unable to fetch headers from this IMAP server version.");
    mutt_sleep (2);	/* pause a moment to let the user see the error */
    goto error_out_0;
  }

  /* instead of downloading all headers and then parsing them, we parse them
   * as they come in. */
  mutt_mktemp (tempfile, sizeof (tempfile));
  if (!(fp = safe_fopen (tempfile, "w+")))
  {
    mutt_error (_("Could not create temporary file %s"), tempfile);
    mutt_sleep (2);
    goto error_out_0;
  }
  unlink (tempfile);

  /* make sure context has room to hold the mailbox */
  while (msn_end > ctx->hdrmax)
    mx_alloc_memory (ctx);
  imap_alloc_msn_index (idata, msn_end);

  idx = ctx->msgcount;
  oldmsgcount = ctx->msgcount;
  idata->reopen &= ~(IMAP_REOPEN_ALLOW|IMAP_NEWMAIL_PENDING);
  idata->newMailCount = 0;

#if USE_HCACHE
  idata->hcache = imap_hcache_open (idata, NULL);

  if (idata->hcache && (msn_begin == 1))
  {
    uid_validity = mutt_hcache_fetch_raw (idata->hcache, "/UIDVALIDITY", imap_hcache_keylen);
    puidnext = mutt_hcache_fetch_raw (idata->hcache, "/UIDNEXT", imap_hcache_keylen);
    if (puidnext)
    {
      uidnext = *puidnext;
      mutt_hcache_free ((void **)&puidnext);
    }
    if (uid_validity && uidnext && *uid_validity == idata->uid_validity)
      evalhc = 1;
    mutt_hcache_free ((void **)&uid_validity);
  }
  if (evalhc)
  {
    /* L10N:
       Comparing the cached data with the IMAP server's data */
    mutt_progress_init (&progress, _("Evaluating cache..."),
			MUTT_PROGRESS_MSG, ReadInc, msn_end);

    snprintf (buf, sizeof (buf),
      "UID FETCH 1:%u (UID FLAGS)", uidnext - 1);

    imap_cmd_start (idata, buf);

    rc = IMAP_CMD_CONTINUE;
    for (msgno = 1; rc == IMAP_CMD_CONTINUE; msgno++)
    {
      mutt_progress_update (&progress, msgno, -1);

      memset (&h, 0, sizeof (h));
      h.data = safe_calloc (1, sizeof (IMAP_HEADER_DATA));
      do
      {
        rc = imap_cmd_step (idata);
        if (rc != IMAP_CMD_CONTINUE)
          break;

        if ((mfhrc = msg_fetch_header (ctx, &h, idata->buf, NULL)) < 0)
          continue;

        if (!h.data->uid)
        {
          dprint (2, (debugfile, "imap_read_headers: skipping hcache FETCH "
                      "response for message number %d missing a UID\n", h.data->msn));
          continue;
        }

        if (h.data->msn < 1 || h.data->msn > msn_end)
        {
          dprint (1, (debugfile, "imap_read_headers: skipping hcache FETCH "
                      "response for unknown message number %d\n", h.data->msn));
          continue;
        }

        if (idata->msn_index[h.data->msn - 1])
        {
          dprint (2, (debugfile, "imap_read_headers: skipping hcache FETCH "
                      "for duplicate message %d\n", h.data->msn));
          continue;
        }

        ctx->hdrs[idx] = imap_hcache_get (idata, h.data->uid);
        if (ctx->hdrs[idx])
        {
          idata->max_msn = MAX (idata->max_msn, h.data->msn);
          idata->msn_index[h.data->msn - 1] = ctx->hdrs[idx];

  	  ctx->hdrs[idx]->index = idx;
  	  /* messages which have not been expunged are ACTIVE (borrowed from mh
  	   * folders) */
  	  ctx->hdrs[idx]->active = 1;
          ctx->hdrs[idx]->read = h.data->read;
          ctx->hdrs[idx]->old = h.data->old;
          ctx->hdrs[idx]->deleted = h.data->deleted;
          ctx->hdrs[idx]->flagged = h.data->flagged;
          ctx->hdrs[idx]->replied = h.data->replied;
          ctx->hdrs[idx]->changed = h.data->changed;
          /*  ctx->hdrs[msgno]->received is restored from mutt_hcache_restore */
          ctx->hdrs[idx]->data = (void *) (h.data);

          ctx->msgcount++;
          ctx->size += ctx->hdrs[idx]->content->length;

          h.data = NULL;
          idx++;
        }
      }
      while (mfhrc == -1);

      imap_free_header_data (&h.data);

      if ((mfhrc < -1) || ((rc != IMAP_CMD_CONTINUE) && (rc != IMAP_CMD_OK)))
      {
        imap_hcache_close (idata);
	goto error_out_1;
      }
    }

    /* Look for the first empty MSN and start there */
    while (msn_begin <= msn_end)
    {
      if (!idata->msn_index[msn_begin -1])
        break;
      msn_begin++;
    }
  }
#endif /* USE_HCACHE */

  mutt_progress_init (&progress, _("Fetching message headers..."),
		      MUTT_PROGRESS_MSG, ReadInc, msn_end);

  while (msn_begin <= msn_end && fetch_msn_end < msn_end)
  {
    char *cmd;
    BUFFER *b;

    b = mutt_buffer_new ();
    if (evalhc)
    {
      /* In case there are holes in the header cache. */
      evalhc = 0;
      imap_generate_seqset (b, idata, msn_begin, msn_end);
    }
    else
      mutt_buffer_printf (b, "%u:%u", msn_begin, msn_end);

    fetch_msn_end = msn_end;
    safe_asprintf (&cmd, "FETCH %s (UID FLAGS INTERNALDATE RFC822.SIZE %s)",
                   b->data, hdrreq);
    imap_cmd_start (idata, cmd);
    FREE (&cmd);
    mutt_buffer_free (&b);

    rc = IMAP_CMD_CONTINUE;
    for (msgno = msn_begin; rc == IMAP_CMD_CONTINUE; msgno++)
    {
      mutt_progress_update (&progress, msgno, -1);

      rewind (fp);
      memset (&h, 0, sizeof (h));
      h.data = safe_calloc (1, sizeof (IMAP_HEADER_DATA));

      /* this DO loop does two things:
       * 1. handles untagged messages, so we can try again on the same msg
       * 2. fetches the tagged response at the end of the last message.
       */
      do
      {
        rc = imap_cmd_step (idata);
        if (rc != IMAP_CMD_CONTINUE)
          break;

        if ((mfhrc = msg_fetch_header (ctx, &h, idata->buf, fp)) < 0)
          continue;

        if (!ftello (fp))
        {
          dprint (2, (debugfile, "msg_fetch_header: ignoring fetch response with no body\n"));
          continue;
        }

        /* make sure we don't get remnants from older larger message headers */
        fputs ("\n\n", fp);

        if (h.data->msn < 1 || h.data->msn > fetch_msn_end)
        {
          dprint (1, (debugfile, "imap_read_headers: skipping FETCH response for "
                      "unknown message number %d\n", h.data->msn));
          continue;
        }

        /* May receive FLAGS updates in a separate untagged response (#2935) */
        if (idata->msn_index[h.data->msn - 1])
        {
          dprint (2, (debugfile, "imap_read_headers: skipping FETCH response for "
                      "duplicate message %d\n", h.data->msn));
          continue;
        }

        ctx->hdrs[idx] = mutt_new_header ();

        idata->max_msn = MAX (idata->max_msn, h.data->msn);
        idata->msn_index[h.data->msn - 1] = ctx->hdrs[idx];

        ctx->hdrs[idx]->index = idx;
        /* messages which have not been expunged are ACTIVE (borrowed from mh
         * folders) */
        ctx->hdrs[idx]->active = 1;
        ctx->hdrs[idx]->read = h.data->read;
        ctx->hdrs[idx]->old = h.data->old;
        ctx->hdrs[idx]->deleted = h.data->deleted;
        ctx->hdrs[idx]->flagged = h.data->flagged;
        ctx->hdrs[idx]->replied = h.data->replied;
        ctx->hdrs[idx]->changed = h.data->changed;
        ctx->hdrs[idx]->received = h.received;
        ctx->hdrs[idx]->data = (void *) (h.data);

        if (maxuid < h.data->uid)
          maxuid = h.data->uid;

        rewind (fp);
        /* NOTE: if Date: header is missing, mutt_read_rfc822_header depends
         *   on h.received being set */
        ctx->hdrs[idx]->env = mutt_read_rfc822_header (fp, ctx->hdrs[idx],
                                                       0, 0);
        /* content built as a side-effect of mutt_read_rfc822_header */
        ctx->hdrs[idx]->content->length = h.content_length;
        ctx->size += h.content_length;

#if USE_HCACHE
        imap_hcache_put (idata, ctx->hdrs[idx]);
#endif /* USE_HCACHE */

        ctx->msgcount++;

        h.data = NULL;
        idx++;
      }
      while (mfhrc == -1);

      imap_free_header_data (&h.data);

      if ((mfhrc < -1) || ((rc != IMAP_CMD_CONTINUE) && (rc != IMAP_CMD_OK)))
      {
#if USE_HCACHE
        imap_hcache_close (idata);
#endif
        goto error_out_1;
      }
    }

    /* In case we get new mail while fetching the headers.
     *
     * Note: The RFC says we shouldn't get any EXPUNGE responses in the
     * middle of a FETCH.  But just to be cautious, use the current state
     * of max_msn, not fetch_msn_end to set the next start range.
     */
    if (idata->reopen & IMAP_NEWMAIL_PENDING)
    {
      /* update to the last value we actually pulled down */
      fetch_msn_end = idata->max_msn;
      msn_begin = idata->max_msn + 1;
      msn_end = idata->newMailCount;
      while (msn_end > ctx->hdrmax)
        mx_alloc_memory (ctx);
      imap_alloc_msn_index (idata, msn_end);
      idata->reopen &= ~IMAP_NEWMAIL_PENDING;
      idata->newMailCount = 0;
    }
  }

  if (maxuid && (status = imap_mboxcache_get (idata, idata->mailbox, 0)) &&
      (status->uidnext < maxuid + 1))
    status->uidnext = maxuid + 1;

#if USE_HCACHE
  mutt_hcache_store_raw (idata->hcache, "/UIDVALIDITY", &idata->uid_validity,
                         sizeof (idata->uid_validity), imap_hcache_keylen);
  if (maxuid && idata->uidnext < maxuid + 1)
  {
    dprint (2, (debugfile, "Overriding UIDNEXT: %u -> %u\n", idata->uidnext, maxuid + 1));
    idata->uidnext = maxuid + 1;
  }
  if (idata->uidnext > 1)
    mutt_hcache_store_raw (idata->hcache, "/UIDNEXT", &idata->uidnext,
			   sizeof (idata->uidnext), imap_hcache_keylen);

  imap_hcache_close (idata);
#endif /* USE_HCACHE */

  if (ctx->msgcount > oldmsgcount)
  {
    /* TODO: it's not clear to me why we are calling mx_alloc_memory
     *       yet again. */
    mx_alloc_memory(ctx);
    mx_update_context (ctx, ctx->msgcount - oldmsgcount);
    imap_update_context (idata, oldmsgcount);
  }

  idata->reopen |= IMAP_REOPEN_ALLOW;

  retval = msn_end;

error_out_1:
  safe_fclose (&fp);

error_out_0:
  FREE (&hdrreq);

  return retval;
}