load_xwd_f2_d8_b8 (const gchar     *filename,
                   FILE            *ifp,
                   L_XWDFILEHEADER *xwdhdr,
                   L_XWDCOLOR      *xwdcolmap)
{
  gint        width, height, linepad, tile_height, scan_lines;
  gint        i, j, ncols;
  gint        grayscale;
  guchar     *dest, *data;
  gint        err = 0;
  gint32      layer_ID, image_ID;
  GeglBuffer *buffer;

#ifdef XWD_DEBUG
  printf ("load_xwd_f2_d8_b8 (%s)\n", filename);
#endif

  width  = xwdhdr->l_pixmap_width;
  height = xwdhdr->l_pixmap_height;

  /* This could also be a grayscale image. Check it */
  grayscale = 0;
  if ((xwdhdr->l_ncolors == 256) && (xwdhdr->l_colormap_entries == 256))
    {
      for (j = 0; j < 256; j++)
        {
          if ((xwdcolmap[j].l_pixel != j)
              || ((xwdcolmap[j].l_red >> 8) != j)
              || ((xwdcolmap[j].l_green >> 8) != j)
              || ((xwdcolmap[j].l_blue >> 8) != j))
            break;
        }

      grayscale = (j == 256);
    }

  image_ID = create_new_image (filename, width, height,
                               grayscale ? GIMP_GRAY : GIMP_INDEXED,
                               grayscale ? GIMP_GRAY_IMAGE : GIMP_INDEXED_IMAGE,
                               &layer_ID, &buffer);

  tile_height = gimp_tile_height ();
  data = g_malloc (tile_height * width);

  if (!grayscale)
    {
      ncols = xwdhdr->l_colormap_entries;
      if (xwdhdr->l_ncolors < ncols) ncols = xwdhdr->l_ncolors;
      if (ncols < 2)
        set_bw_color_table (image_ID);
      else
        set_color_table (image_ID, xwdhdr, xwdcolmap);
    }

  linepad = xwdhdr->l_bytes_per_line - xwdhdr->l_pixmap_width;
  if (linepad < 0)
    linepad = 0;

  dest = data;
  scan_lines = 0;

  for (i = 0; i < height; i++)
    {
      if (fread (dest, 1, width, ifp) != width)
        {
          err = 1;
          break;
        }
      dest += width;

      for (j = 0; j < linepad; j++)
        getc (ifp);

      scan_lines++;

      if ((i % 20) == 0)
        gimp_progress_update ((gdouble) (i + 1) / (gdouble) height);

      if ((scan_lines == tile_height) || ((i+1) == height))
        {
          gegl_buffer_set (buffer, GEGL_RECTANGLE (0, i - scan_lines + 1,
                                                   width, scan_lines), 0,
                           NULL, data, GEGL_AUTO_ROWSTRIDE);

          scan_lines = 0;
          dest = data;
        }
    }

  g_free (data);

  if (err)
    g_message (_("EOF encountered on reading"));

  g_object_unref (buffer);

  return err ? -1 : image_ID;
}