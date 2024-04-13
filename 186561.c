_dbus_printf_string_upper_bound (const char *format,
                                 va_list     args)
{
  char static_buf[1024];
  int bufsize = sizeof (static_buf);
  int len;
  va_list args_copy;

  DBUS_VA_COPY (args_copy, args);
  len = vsnprintf (static_buf, bufsize, format, args_copy);
  va_end (args_copy);

  /* If vsnprintf() returned non-negative, then either the string fits in
   * static_buf, or this OS has the POSIX and C99 behaviour where vsnprintf
   * returns the number of characters that were needed, or this OS returns the
   * truncated length.
   *
   * We ignore the possibility that snprintf might just ignore the length and
   * overrun the buffer (64-bit Solaris 7), because that's pathological.
   * If your libc is really that bad, come back when you have a better one. */
  if (len == bufsize)
    {
      /* This could be the truncated length (Tru64 and IRIX have this bug),
       * or the real length could be coincidentally the same. Which is it?
       * If vsnprintf returns the truncated length, we'll go to the slow
       * path. */
      DBUS_VA_COPY (args_copy, args);

      if (vsnprintf (static_buf, 1, format, args_copy) == 1)
        len = -1;

      va_end (args_copy);
    }

  /* If vsnprintf() returned negative, we have to do more work.
   * HP-UX returns negative. */
  while (len < 0)
    {
      char *buf;

      bufsize *= 2;

      buf = dbus_malloc (bufsize);

      if (buf == NULL)
        return -1;

      DBUS_VA_COPY (args_copy, args);
      len = vsnprintf (buf, bufsize, format, args_copy);
      va_end (args_copy);

      dbus_free (buf);

      /* If the reported length is exactly the buffer size, round up to the
       * next size, in case vsnprintf has been returning the truncated
       * length */
      if (len == bufsize)
        len = -1;
    }

  return len;
}