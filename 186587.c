_dbus_init_system_log (const char   *tag,
                       DBusLogFlags  flags)
{
  /* We never want to turn off logging completely */
  _dbus_assert (
      (flags & (DBUS_LOG_FLAGS_STDERR | DBUS_LOG_FLAGS_SYSTEM_LOG)) != 0);

  syslog_tag = tag;

#ifdef HAVE_SYSLOG_H
  log_flags = flags;

  if (log_flags & DBUS_LOG_FLAGS_SYSTEM_LOG)
    openlog (tag, LOG_PID, LOG_DAEMON);
#endif
}