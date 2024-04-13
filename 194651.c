receive_bomb_out(uschar *reason, uschar *msg)
{
  static BOOL already_bombing_out;
/* The smtp_notquit_exit() below can call ACLs which can trigger recursive
timeouts, if someone has something slow in their quit ACL.  Since the only
things we should be doing are to close down cleanly ASAP, on the second
pass we also close down stuff that might be opened again, before bypassing
the ACL call and exiting. */

/* If spool_name is set, it contains the name of the data file that is being
written. Unlink it before closing so that it cannot be picked up by a delivery
process. Ensure that any header file is also removed. */

if (spool_name[0] != '\0')
  {
  Uunlink(spool_name);
  spool_name[Ustrlen(spool_name) - 1] = 'H';
  Uunlink(spool_name);
  spool_name[0] = '\0';
  }

/* Now close the file if it is open, either as a fd or a stream. */

if (data_file != NULL)
  {
  (void)fclose(data_file);
  data_file = NULL;
} else if (data_fd >= 0) {
  (void)close(data_fd);
  data_fd = -1;
  }

/* Attempt to close down an SMTP connection tidily. For non-batched SMTP, call
smtp_notquit_exit(), which runs the NOTQUIT ACL, if present, and handles the
SMTP response. */

if (!already_bombing_out)
  {
  already_bombing_out = TRUE;
  if (smtp_input)
    {
    if (smtp_batched_input)
      moan_smtp_batch(NULL, "421 %s - message abandoned", msg);  /* No return */
    smtp_notquit_exit(reason, US"421", US"%s %s - closing connection.",
      smtp_active_hostname, msg);
    }
  }

/* Exit from the program (non-BSMTP cases) */

exim_exit(EXIT_FAILURE, NULL);
}