cmd_disconnect (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);

  (void)line;

  ctrl->server_local->disconnect_allowed = 1;
  return 0;
}