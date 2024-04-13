int LibRaw::adjust_sizes_info_only(void)
{
  CHECK_ORDER_LOW(LIBRAW_PROGRESS_IDENTIFY);

  raw2image_start();
  if (O.use_fuji_rotate)
    {
      if (IO.fuji_width) 
        {
          IO.fuji_width = (IO.fuji_width - 1 + IO.shrink) >> IO.shrink;
          S.iwidth = (ushort)(IO.fuji_width / sqrt(0.5));
          S.iheight = (ushort)( (S.iheight - IO.fuji_width) / sqrt(0.5));
        } 
      else 
        {
          if (S.pixel_aspect < 1) S.iheight = (ushort)( S.iheight / S.pixel_aspect + 0.5);
          if (S.pixel_aspect > 1) S.iwidth  = (ushort) (S.iwidth  * S.pixel_aspect + 0.5);
        }
    }
  SET_PROC_FLAG(LIBRAW_PROGRESS_FUJI_ROTATE);
  if ( S.flip & 4)
    {
      unsigned short t = S.iheight;
      S.iheight=S.iwidth;
      S.iwidth = t;
      SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
    }
  return 0;
}