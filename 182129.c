MagickExport void XFreeStandardColormap(Display *display,
  const XVisualInfo *visual_info,XStandardColormap *map_info,XPixelInfo *pixel)
{
  /*
    Free colormap.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  (void) XFlush(display);
  if (map_info->colormap != (Colormap) NULL)
    {
      if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
        (void) XFreeColormap(display,map_info->colormap);
      else
        if (pixel != (XPixelInfo *) NULL)
          if ((visual_info->klass != TrueColor) &&
              (visual_info->klass != DirectColor))
            (void) XFreeColors(display,map_info->colormap,pixel->pixels,
              (int) pixel->colors,0);
    }
  map_info->colormap=(Colormap) NULL;
  if (pixel != (XPixelInfo *) NULL)
    {
      if (pixel->pixels != (unsigned long *) NULL)
        pixel->pixels=(unsigned long *) RelinquishMagickMemory(pixel->pixels);
      pixel->pixels=(unsigned long *) NULL;
    }
}