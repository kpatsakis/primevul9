on_sigterm_pipe (GIOChannel  *channel,
                 GIOCondition condition,
                 gpointer     data)
{
  A11yBusLauncher *app = data;
  
  g_main_loop_quit (app->loop);

  return FALSE;
}