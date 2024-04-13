MagickPrivate void XConfigureImageColormap(Display *display,
  XResourceInfo *resource_info,XWindows *windows,Image *image,
  ExceptionInfo *exception)
{
  Colormap
    colormap;

  /*
    Make standard colormap.
  */
  XSetCursorState(display,windows,MagickTrue);
  XCheckRefreshWindows(display,windows);
  XMakeStandardColormap(display,windows->visual_info,resource_info,image,
    windows->map_info,windows->pixel_info,exception);
  colormap=windows->map_info->colormap;
  (void) XSetWindowColormap(display,windows->image.id,colormap);
  (void) XSetWindowColormap(display,windows->command.id,colormap);
  (void) XSetWindowColormap(display,windows->widget.id,colormap);
  if (windows->magnify.mapped != MagickFalse)
    (void) XSetWindowColormap(display,windows->magnify.id,colormap);
  if (windows->pan.mapped != MagickFalse)
    (void) XSetWindowColormap(display,windows->pan.id,colormap);
  XSetCursorState(display,windows,MagickFalse);
  XClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_colormap,CurrentTime);
}