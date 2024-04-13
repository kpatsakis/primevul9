static _Bool have_gcrypt (void) /* {{{ */
{
  static _Bool result = 0;
  static _Bool need_init = 1;

  if (!need_init)
    return (result);
  need_init = 0;

#if HAVE_LIBGCRYPT
# if GCRYPT_VERSION_NUMBER < 0x010600
  if (gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread))
    return (0);
# endif

  if (!gcry_check_version (GCRYPT_VERSION))
    return (0);

  if (!gcry_control (GCRYCTL_INIT_SECMEM, 32768, 0))
    return (0);

  gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);

  result = 1;
  return (1);
#else
  return(0);
#endif
} /* }}} _Bool have_gcrypt */