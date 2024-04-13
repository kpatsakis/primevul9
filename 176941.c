save_rgb (FILE   *ofp,
          gint32  image_ID,
          gint32  drawable_ID)
{
  gint             height, width, linepad, tile_height, i;
  glong            tmp = 0;
  guchar          *data, *src;
  L_XWDFILEHEADER  xwdhdr;
  const Babl      *format;
  GeglBuffer      *buffer;

#ifdef XWD_DEBUG
  printf ("save_rgb ()\n");
#endif

  buffer        = gimp_drawable_get_buffer (drawable_ID);
  width         = gegl_buffer_get_width  (buffer);
  height        = gegl_buffer_get_height (buffer);
  tile_height   = gimp_tile_height ();
  format        = babl_format ("R'G'B' u8");

  /* allocate a buffer for retrieving information from the pixel region  */
  src = data = g_new (guchar,
                      tile_height * width *
                      babl_format_get_bytes_per_pixel (format));

  linepad = (width * 3) % 4;
  if (linepad)
    linepad = 4 - linepad;

  /* Fill in the XWD header (header_size is evaluated by write_xwd_hdr ()) */
  xwdhdr.l_header_size      = 0;
  xwdhdr.l_file_version     = 7;
  xwdhdr.l_pixmap_format    = 2;
  xwdhdr.l_pixmap_depth     = 24;
  xwdhdr.l_pixmap_width     = width;
  xwdhdr.l_pixmap_height    = height;
  xwdhdr.l_xoffset          = 0;
  xwdhdr.l_byte_order       = 1;

  xwdhdr.l_bitmap_unit      = 32;
  xwdhdr.l_bitmap_bit_order = 1;
  xwdhdr.l_bitmap_pad       = 32;
  xwdhdr.l_bits_per_pixel   = 24;

  xwdhdr.l_bytes_per_line   = width * 3 + linepad;
  xwdhdr.l_visual_class     = 5;
  xwdhdr.l_red_mask         = 0xff0000;
  xwdhdr.l_green_mask       = 0x00ff00;
  xwdhdr.l_blue_mask        = 0x0000ff;
  xwdhdr.l_bits_per_rgb     = 8;
  xwdhdr.l_colormap_entries = 0;
  xwdhdr.l_ncolors          = 0;
  xwdhdr.l_window_width     = width;
  xwdhdr.l_window_height    = height;
  xwdhdr.l_window_x         = 64;
  xwdhdr.l_window_y         = 64;
  xwdhdr.l_window_bdrwidth  = 0;

  write_xwd_header (ofp, &xwdhdr);

  for (i = 0; i < height; i++)
    {
      if ((i % tile_height) == 0)   /* Get more data */
        {
          gint scan_lines = (i + tile_height - 1 < height) ? tile_height : (height - i);

          gegl_buffer_get (buffer, GEGL_RECTANGLE (0, i, width, scan_lines), 1.0,
                           format, data,
                           GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

          src = data;
        }

      fwrite (src, width * 3, 1, ofp);

      if (linepad)
        fwrite ((char *)&tmp, linepad, 1, ofp);

      src += width * 3;

      if ((i % 20) == 0)
        gimp_progress_update ((gdouble) i / (gdouble) height);
    }

  g_free (data);

  g_object_unref (buffer);

  if (ferror (ofp))
    {
      g_message (_("Error during writing rgb image"));
      return FALSE;
    }

  return TRUE;
}