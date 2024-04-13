_gcry_assert_failed (const char *expr, const char *file, int line,
		     const char *func)
  
{
  grub_fatal ("assertion %s at %s:%d (%s) failed\n", expr, file, line, func);
}