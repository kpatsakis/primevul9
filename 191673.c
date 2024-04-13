static void network_init_gcrypt (void) /* {{{ */
{
  gcry_error_t err;

  /* http://lists.gnupg.org/pipermail/gcrypt-devel/2003-August/000458.html
   * Because you can't know in a library whether another library has
   * already initialized the library */
  if (gcry_control (GCRYCTL_ANY_INITIALIZATION_P))
    return;

 /* http://www.gnupg.org/documentation/manuals/gcrypt/Multi_002dThreading.html
  * To ensure thread-safety, it's important to set GCRYCTL_SET_THREAD_CBS
  * *before* initalizing Libgcrypt with gcry_check_version(), which itself must
  * be called before any other gcry_* function. GCRYCTL_ANY_INITIALIZATION_P
  * above doesn't count, as it doesn't implicitly initalize Libgcrypt.
  *
  * tl;dr: keep all these gry_* statements in this exact order please. */
# if GCRYPT_VERSION_NUMBER < 0x010600
  err = gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
  if (err)
  {
    ERROR ("network plugin: gcry_control (GCRYCTL_SET_THREAD_CBS) failed: %s", gcry_strerror (err));
    abort ();
  }
# endif

  gcry_check_version (NULL);

  err = gcry_control (GCRYCTL_INIT_SECMEM, 32768);
  if (err)
  {
    ERROR ("network plugin: gcry_control (GCRYCTL_SET_THREAD_CBS) failed: %s", gcry_strerror (err));
    abort ();
  }

  gcry_control (GCRYCTL_INITIALIZATION_FINISHED);
} /* }}} void network_init_gcrypt */