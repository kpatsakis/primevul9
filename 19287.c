scd_update_reader_status_file (void)
{
  int err;
  err = npth_mutex_lock (&status_file_update_lock);
  if (err)
    return; /* locked - give up. */
  update_reader_status_file (1);
  err = npth_mutex_unlock (&status_file_update_lock);
  if (err)
    log_error ("failed to release status_file_update lock: %s\n",
	       strerror (err));
}