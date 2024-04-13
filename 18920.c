main (int    argc,
      char **argv)
{
  GError *error = NULL;
  GMainLoop *loop;
  GDBusConnection *session_bus;
  int name_owner_id;
  gboolean a11y_set = FALSE;
  gboolean screen_reader_set = FALSE;
  gint i;

  if (already_running ())
    return 0;

  _global_app = g_slice_new0 (A11yBusLauncher);
  _global_app->loop = g_main_loop_new (NULL, FALSE);

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "--launch-immediately"))
        _global_app->launch_immediately = TRUE;
      else if (sscanf (argv[i], "--a11y=%d", &_global_app->a11y_enabled) == 1)
        a11y_set = TRUE;
      else if (sscanf (argv[i], "--screen-reader=%d",
                       &_global_app->screen_reader_enabled) == 1)
        screen_reader_set = TRUE;
    else
      g_error ("usage: %s [--launch-immediately] [--a11y=0|1] [--screen-reader=0|1]", argv[0]);
    }

  _global_app->interface_schema = get_schema ("org.gnome.desktop.interface");
  _global_app->a11y_schema = get_schema ("org.gnome.desktop.a11y.applications");

  if (!a11y_set)
    {
      _global_app->a11y_enabled = _global_app->interface_schema
                                  ? g_settings_get_boolean (_global_app->interface_schema, "toolkit-accessibility")
                                  : _global_app->launch_immediately;
    }

  if (!screen_reader_set)
    {
      _global_app->screen_reader_enabled = _global_app->a11y_schema
                                  ? g_settings_get_boolean (_global_app->a11y_schema, "screen-reader-enabled")
                                  : FALSE;
    }

  if (_global_app->interface_schema)
    g_signal_connect (_global_app->interface_schema,
                      "changed::toolkit-accessibility",
                      G_CALLBACK (gsettings_key_changed), _global_app);

  if (_global_app->a11y_schema)
    g_signal_connect (_global_app->a11y_schema,
                      "changed::screen-reader-enabled",
                      G_CALLBACK (gsettings_key_changed), _global_app);

  init_sigterm_handling (_global_app);

  introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, NULL);
  g_assert (introspection_data != NULL);

  name_owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                                  "org.a11y.Bus",
                                  G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT,
                                  on_bus_acquired,
                                  on_name_acquired,
                                  on_name_lost,
                                  _global_app,
                                  NULL);

  g_main_loop_run (_global_app->loop);

  if (_global_app->a11y_bus_pid > 0)
    kill (_global_app->a11y_bus_pid, SIGTERM);

  /* Clear the X property if our bus is gone; in the case where e.g. 
   * GDM is launching a login on an X server it was using before,
   * we don't want early login processes to pick up the stale address.
   */
#ifdef HAVE_X11
  {
    Display *display = XOpenDisplay (NULL);
    if (display)
      {
        Atom bus_address_atom = XInternAtom (display, "AT_SPI_BUS", False);
        XDeleteProperty (display,
                         XDefaultRootWindow (display),
                         bus_address_atom);

        XFlush (display);
        XCloseDisplay (display);
      }
  }
#endif

  if (_global_app->a11y_launch_error_message)
    {
      g_printerr ("Failed to launch bus: %s", _global_app->a11y_launch_error_message);
      return 1;
    }
  return 0;
}