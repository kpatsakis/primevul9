MagickExport void XClientMessage(Display *display,const Window window,
  const Atom protocol,const Atom reason,const Time timestamp)
{
  XClientMessageEvent
    client_event;

  assert(display != (Display *) NULL);
  (void) memset(&client_event,0,sizeof(client_event));
  client_event.type=ClientMessage;
  client_event.window=window;
  client_event.message_type=protocol;
  client_event.format=32;
  client_event.data.l[0]=(long) reason;
  client_event.data.l[1]=(long) timestamp;
  (void) XSendEvent(display,window,MagickFalse,NoEventMask,(XEvent *) &client_event);
}