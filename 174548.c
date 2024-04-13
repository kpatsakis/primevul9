save_image (GFile   *file,
            gint32   image_ID,
            gint32   drawable_ID,
            GError **error)
{
  GOutputStream *output;
  BrushHeader    bh;
  guchar        *brush_buf;
  GeglBuffer    *buffer;
  const Babl    *format;
  gint           line;
  gint           x;
  gint           bpp;
  gint           file_bpp;
  gint           width;
  gint           height;
  GimpRGB        gray, white;

  gimp_rgba_set_uchar (&white, 255, 255, 255, 255);

  switch (gimp_drawable_type (drawable_ID))
    {
    case GIMP_GRAY_IMAGE:
      file_bpp = 1;
      format = babl_format ("Y' u8");
      break;

    case GIMP_GRAYA_IMAGE:
      file_bpp = 1;
      format = babl_format ("Y'A u8");
      break;

    default:
      file_bpp = 4;
      format = babl_format ("R'G'B'A u8");
      break;
    }

  bpp = babl_format_get_bytes_per_pixel (format);

  gimp_progress_init_printf (_("Exporting '%s'"),
                             g_file_get_parse_name (file));

  output = G_OUTPUT_STREAM (g_file_replace (file,
                                            NULL, FALSE, G_FILE_CREATE_NONE,
                                            NULL, error));
  if (! output)
    return FALSE;

  buffer = gimp_drawable_get_buffer (drawable_ID);

  width  = gimp_drawable_width  (drawable_ID);
  height = gimp_drawable_height (drawable_ID);

  bh.header_size  = g_htonl (sizeof (BrushHeader) +
                             strlen (info.description) + 1);
  bh.version      = g_htonl (2);
  bh.width        = g_htonl (width);
  bh.height       = g_htonl (height);
  bh.bytes        = g_htonl (file_bpp);
  bh.magic_number = g_htonl (GBRUSH_MAGIC);
  bh.spacing      = g_htonl (info.spacing);

  if (! g_output_stream_write_all (output, &bh, sizeof (BrushHeader),
                                   NULL, NULL, error))
    {
      g_object_unref (output);
      return FALSE;
    }

  if (! g_output_stream_write_all (output,
                                   info.description,
                                   strlen (info.description) + 1,
                                   NULL, NULL, error))
    {
      g_object_unref (output);
      return FALSE;
    }

  brush_buf = g_new (guchar, width * bpp);

  for (line = 0; line < height; line++)
    {
      gegl_buffer_get (buffer, GEGL_RECTANGLE (0, line, width, 1), 1.0,
                       format, brush_buf,
                       GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

      switch (bpp)
        {
        case 1:
          /*  invert  */
          for (x = 0; x < width; x++)
            brush_buf[x] = 255 - brush_buf[x];
          break;

        case 2:
          for (x = 0; x < width; x++)
            {
              /*  apply alpha channel  */
              gimp_rgba_set_uchar (&gray,
                                   brush_buf[2 * x],
                                   brush_buf[2 * x],
                                   brush_buf[2 * x],
                                   brush_buf[2 * x + 1]);
              gimp_rgb_composite (&gray, &white, GIMP_RGB_COMPOSITE_BEHIND);
              gimp_rgba_get_uchar (&gray, &brush_buf[x], NULL, NULL, NULL);
              /* invert */
              brush_buf[x] = 255 - brush_buf[x];
            }
          break;
        }

      if (! g_output_stream_write_all (output, brush_buf, width * file_bpp,
                                       NULL, NULL, error))
        {
          g_free (brush_buf);
          g_object_unref (output);
          return FALSE;
        }

      gimp_progress_update ((gdouble) line / (gdouble) height);
    }

  g_free (brush_buf);
  g_object_unref (buffer);
  g_object_unref (output);

  gimp_progress_update (1.0);

  return TRUE;
}