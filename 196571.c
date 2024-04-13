void _gcry_log_error (const char *fmt, ...)
{
  va_list args;
  const char *debug = grub_env_get ("debug");

  if (! debug)
    return;

  if (grub_strword (debug, "all") || grub_strword (debug, "gcrypt"))
    {
      grub_printf ("gcrypt error: ");
      va_start (args, fmt);
      grub_vprintf (fmt, args);
      va_end (args);
      grub_refresh ();
    }
}