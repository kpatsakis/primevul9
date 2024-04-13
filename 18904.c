init_sigterm_handling (A11yBusLauncher *app)
{
  GIOChannel *sigterm_channel;

  if (pipe (sigterm_pipefd) < 0)
    g_error ("Failed to create pipe: %s", strerror (errno));
  signal (SIGTERM, sigterm_handler);

  sigterm_channel = g_io_channel_unix_new (sigterm_pipefd[0]);
  g_io_add_watch (sigterm_channel,
                  G_IO_IN | G_IO_ERR | G_IO_HUP,
                  on_sigterm_pipe,
                  app);
}