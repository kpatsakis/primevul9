_dbus_strerror (int error_number)
{
  const char *msg;

  msg = strerror (error_number);
  if (msg == NULL)
    msg = "unknown";

  return msg;
}