ensure_a11y_bus (A11yBusLauncher *app)
{
  GPid pid;
  char *argv[] = { DBUS_DAEMON, NULL, "--nofork", "--print-address", "3", NULL };
  char addr_buf[2048];
  GError *error = NULL;
  const char *config_path = NULL;

  if (app->a11y_bus_pid != 0)
    return FALSE;

  if (g_file_test (SYSCONFDIR"/at-spi2/accessibility.conf", G_FILE_TEST_EXISTS))
      config_path = "--config-file="SYSCONFDIR"/at-spi2/accessibility.conf";
  else
      config_path = "--config-file="DATADIR"/defaults/at-spi2/accessibility.conf";

  argv[1] = config_path;

  if (pipe (app->pipefd) < 0)
    g_error ("Failed to create pipe: %s", strerror (errno));
  
  if (!g_spawn_async (NULL,
                      argv,
                      NULL,
                      G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
                      setup_bus_child,
                      app,
                      &pid,
                      &error))
    {
      app->a11y_bus_pid = -1;
      app->a11y_launch_error_message = g_strdup (error->message);
      g_clear_error (&error);
      goto error;
    }

  close (app->pipefd[1]);
  app->pipefd[1] = -1;

  g_child_watch_add (pid, on_bus_exited, app);

  app->state = A11Y_BUS_STATE_READING_ADDRESS;
  app->a11y_bus_pid = pid;
  g_debug ("Launched a11y bus, child is %ld", (long) pid);
  if (!unix_read_all_fd_to_string (app->pipefd[0], addr_buf, sizeof (addr_buf)))
    {
      app->a11y_launch_error_message = g_strdup_printf ("Failed to read address: %s", strerror (errno));
      kill (app->a11y_bus_pid, SIGTERM);
      goto error;
    }
  close (app->pipefd[0]);
  app->pipefd[0] = -1;
  app->state = A11Y_BUS_STATE_RUNNING;

  /* Trim the trailing newline */
  app->a11y_bus_address = g_strchomp (g_strdup (addr_buf));
  g_debug ("a11y bus address: %s", app->a11y_bus_address);

#ifdef HAVE_X11
  {
    Display *display = XOpenDisplay (NULL);
    if (display)
      {
        Atom bus_address_atom = XInternAtom (display, "AT_SPI_BUS", False);
        XChangeProperty (display,
                         XDefaultRootWindow (display),
                         bus_address_atom,
                         XA_STRING, 8, PropModeReplace,
                         (guchar *) app->a11y_bus_address, strlen (app->a11y_bus_address));
        XFlush (display);
        XCloseDisplay (display);
      }
  }
#endif

  return TRUE;
  
 error:
  close (app->pipefd[0]);
  close (app->pipefd[1]);
  app->state = A11Y_BUS_STATE_ERROR;

  return FALSE;
}