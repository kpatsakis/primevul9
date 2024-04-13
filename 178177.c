lock_pool (void)
{
  int err;

  err = gpgrt_lock_lock (&pool_lock);
  if (err)
    log_fatal ("failed to acquire the pool lock: %s\n", gpg_strerror (err));
  pool_is_locked = 1;
}