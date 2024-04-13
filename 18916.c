on_bus_exited (GPid     pid,
               gint     status,
               gpointer data)
{
  A11yBusLauncher *app = data;
  
  app->a11y_bus_pid = -1;
  app->state = A11Y_BUS_STATE_ERROR;
  if (app->a11y_launch_error_message == NULL)
    {
      if (WIFEXITED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus exited with code %d", WEXITSTATUS (status));
      else if (WIFSIGNALED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus killed by signal %d", WTERMSIG (status));
      else if (WIFSTOPPED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus stopped by signal %d", WSTOPSIG (status));
    }
  g_main_loop_quit (app->loop);
} 