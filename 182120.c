MagickExport MagickBooleanType XRemoteCommand(Display *display,
  const char *window,const char *filename)
{
  Atom
    remote_atom;

  Window
    remote_window,
    root_window;

  assert(filename != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  if (display == (Display *) NULL)
    display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    {
      ThrowXWindowException(XServerError,"UnableToOpenXServer",filename);
      return(MagickFalse);
    }
  remote_atom=XInternAtom(display,"IM_PROTOCOLS",MagickFalse);
  remote_window=(Window) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  if (window != (char *) NULL)
    {
      /*
        Search window hierarchy and identify any clients by name or ID.
      */
      if (isdigit((int) ((unsigned char) *window)) != 0)
        remote_window=XWindowByID(display,root_window,(Window)
          strtol((char *) window,(char **) NULL,0));
      if (remote_window == (Window) NULL)
        remote_window=XWindowByName(display,root_window,window);
    }
  if (remote_window == (Window) NULL)
    remote_window=XWindowByProperty(display,root_window,remote_atom);
  if (remote_window == (Window) NULL)
    {
      ThrowXWindowException(XServerError,"UnableToConnectToRemoteDisplay",
        filename);
      return(MagickFalse);
    }
  /*
    Send remote command.
  */
  remote_atom=XInternAtom(display,"IM_REMOTE_COMMAND",MagickFalse);
  (void) XChangeProperty(display,remote_window,remote_atom,XA_STRING,8,
    PropModeReplace,(unsigned char *) filename,(int) strlen(filename));
  (void) XSync(display,MagickFalse);
  return(MagickTrue);
}