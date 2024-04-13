send_status_direct (ctrl_t ctrl, const char *keyword, const char *args)
{
  assuan_context_t ctx = ctrl->server_local->assuan_ctx;

  if (strchr (args, '\n'))
    log_error ("error: LF detected in status line - not sending\n");
  else
    assuan_write_status (ctx, keyword, args);
}