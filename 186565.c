_dbus_logv (DBusSystemLogSeverity  severity,
            const char            *msg,
            va_list                args)
{
  va_list tmp;
#ifdef HAVE_SYSLOG_H
  if (log_flags & DBUS_LOG_FLAGS_SYSTEM_LOG)
    {
      int flags;
      switch (severity)
        {
          case DBUS_SYSTEM_LOG_INFO:
            flags =  LOG_DAEMON | LOG_INFO;
            break;
          case DBUS_SYSTEM_LOG_WARNING:
            flags =  LOG_DAEMON | LOG_WARNING;
            break;
          case DBUS_SYSTEM_LOG_SECURITY:
            flags = LOG_AUTH | LOG_NOTICE;
            break;
          case DBUS_SYSTEM_LOG_ERROR:
            flags = LOG_DAEMON|LOG_CRIT;
            break;
          default:
            _dbus_assert_not_reached ("invalid log severity");
        }

      DBUS_VA_COPY (tmp, args);
      vsyslog (flags, msg, tmp);
      va_end (tmp);
    }

  /* If we don't have syslog.h, we always behave as though stderr was in
   * the flags */
  if (log_flags & DBUS_LOG_FLAGS_STDERR)
#endif
    {
      DBUS_VA_COPY (tmp, args);
      fprintf (stderr, "%s[" DBUS_PID_FORMAT "]: ", syslog_tag, _dbus_getpid ());
      vfprintf (stderr, msg, tmp);
      fputc ('\n', stderr);
      va_end (tmp);
    }
}