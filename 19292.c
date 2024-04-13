reset_notify (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);

  (void) line;

  do_reset (ctrl, 1);
  return 0;
}