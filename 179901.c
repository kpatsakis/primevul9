get_bounding_box (GeglOperation *operation)
{
  GeglProperties   *o = GEGL_PROPERTIES (operation);
  GeglRectangle result = {0,0,0,0};
  pnm_struct    img;
  FILE         *fp;

  img.bpc = 1;

  fp = (!strcmp (o->path, "-") ? stdin : fopen (o->path,"rb") );

  if (!fp)
    return result;

  if (!ppm_load_read_header (fp, &img))
    goto out;

  if (img.bpc == 1)
    {
      if (img.channels == 3)
        gegl_operation_set_format (operation, "output",
                                   babl_format ("R'G'B' u8"));
      else
        gegl_operation_set_format (operation, "output",
                                   babl_format ("Y' u8"));
    }
  else if (img.bpc == 2)
    {
      if (img.channels == 3)
        gegl_operation_set_format (operation, "output",
                                   babl_format ("R'G'B' u16"));
      else
        gegl_operation_set_format (operation, "output",
                                   babl_format ("Y' u16"));
    }
  else
    g_warning ("%s: Programmer stupidity error", G_STRLOC);

  result.width = img.width;
  result.height = img.height;

 out:
  if (stdin != fp)
    fclose (fp);

  return result;
}