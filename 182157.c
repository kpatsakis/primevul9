MagickExport void XGetMapInfo(const XVisualInfo *visual_info,
  const Colormap colormap,XStandardColormap *map_info)
{
  /*
    Initialize map info.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  map_info->colormap=colormap;
  map_info->red_max=visual_info->red_mask;
  map_info->red_mult=(size_t) (map_info->red_max != 0 ? 1 : 0);
  if (map_info->red_max != 0)
    while ((map_info->red_max & 0x01) == 0)
    {
      map_info->red_max>>=1;
      map_info->red_mult<<=1;
    }
  map_info->green_max=visual_info->green_mask;
  map_info->green_mult=(size_t) (map_info->green_max != 0 ? 1 : 0);
  if (map_info->green_max != 0)
    while ((map_info->green_max & 0x01) == 0)
    {
      map_info->green_max>>=1;
      map_info->green_mult<<=1;
    }
  map_info->blue_max=visual_info->blue_mask;
  map_info->blue_mult=(size_t) (map_info->blue_max != 0 ? 1 : 0);
  if (map_info->blue_max != 0)
    while ((map_info->blue_max & 0x01) == 0)
    {
      map_info->blue_max>>=1;
      map_info->blue_mult<<=1;
    }
  map_info->base_pixel=0;
}