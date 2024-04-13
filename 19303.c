cmd_restart (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);

  (void)line;

  if (ctrl->app_ctx)
    {
      release_application (ctrl->app_ctx);
      ctrl->app_ctx = NULL;
    }
  if (locked_session && ctrl->server_local == locked_session)
    {
      locked_session = NULL;
      log_info ("implicitly unlocking due to RESTART\n");
    }
  return 0;
}