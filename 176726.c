void LibRaw::get_mem_image_format(int *width, int *height, int *colors,
                                  int *bps) const

{
  *width = S.width;
  *height = S.height;
  if (imgdata.progress_flags < LIBRAW_PROGRESS_FUJI_ROTATE)
  {
    if (O.use_fuji_rotate)
    {
      if (IO.fuji_width)
      {
        int fuji_width = (IO.fuji_width - 1 + IO.shrink) >> IO.shrink;
        *width = (ushort)(fuji_width / sqrt(0.5));
        *height = (ushort)((*height - fuji_width) / sqrt(0.5));
      }
      else
      {
        if (S.pixel_aspect < 0.995)
          *height = (ushort)(*height / S.pixel_aspect + 0.5);
        if (S.pixel_aspect > 1.005)
          *width = (ushort)(*width * S.pixel_aspect + 0.5);
      }
    }
  }
  if (S.flip & 4)
  {
    std::swap(*width, *height);
  }
  *colors = P1.colors;
  *bps = O.output_bps;
}