already_running ()
{
#ifdef HAVE_X11
  Atom AT_SPI_BUS;
  Atom actual_type;
  Display *bridge_display;
  int actual_format;
  unsigned char *data = NULL;
  unsigned long nitems;
  unsigned long leftover;
  gboolean result = FALSE;

  bridge_display = XOpenDisplay (NULL);
  if (!bridge_display)
	      return FALSE;
      
  AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False);
  XGetWindowProperty (bridge_display,
		      XDefaultRootWindow (bridge_display),
		      AT_SPI_BUS, 0L,
		      (long) BUFSIZ, False,
		      (Atom) 31, &actual_type, &actual_format,
		      &nitems, &leftover, &data);

  if (data)
  {
    GDBusConnection *bus;
    bus = g_dbus_connection_new_for_address_sync ((const gchar *)data, 0,
                                                  NULL, NULL, NULL);
    if (bus != NULL)
      {
        result = TRUE;
        g_object_unref (bus);
      }
  }

  XCloseDisplay (bridge_display);
  return result;
#else
  return FALSE;
#endif
}