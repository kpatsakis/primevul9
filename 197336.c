MagickPrivate void XSetCursorState(Display *display,XWindows *windows,
  const MagickStatusType state)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  if (state)
    {
      (void) XCheckDefineCursor(display,windows->image.id,
        windows->image.busy_cursor);
      (void) XCheckDefineCursor(display,windows->pan.id,
        windows->pan.busy_cursor);
      (void) XCheckDefineCursor(display,windows->magnify.id,
        windows->magnify.busy_cursor);
      (void) XCheckDefineCursor(display,windows->command.id,
        windows->command.busy_cursor);
    }
  else
    {
      (void) XCheckDefineCursor(display,windows->image.id,
        windows->image.cursor);
      (void) XCheckDefineCursor(display,windows->pan.id,windows->pan.cursor);
      (void) XCheckDefineCursor(display,windows->magnify.id,
        windows->magnify.cursor);
      (void) XCheckDefineCursor(display,windows->command.id,
        windows->command.cursor);
      (void) XCheckDefineCursor(display,windows->command.id,
        windows->widget.cursor);
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
    }
  windows->info.mapped=MagickFalse;
}