load_xwd_f2_d1_b1 (const gchar     *filename,
                   FILE            *ifp,
                   L_XWDFILEHEADER *xwdhdr,
                   L_XWDCOLOR      *xwdcolmap)
{
  register int     pix8;
  register guchar *dest, *src;
  guchar           c1, c2, c3, c4;
  gint             width, height, scan_lines, tile_height;
  gint             i, j, ncols;
  gchar           *temp;
  guchar           bit2byte[256 * 8];
  guchar          *data, *scanline;
  gint             err = 0;
  gint32           layer_ID, image_ID;
  GeglBuffer      *buffer;

#ifdef XWD_DEBUG
  printf ("load_xwd_f2_d1_b1 (%s)\n", filename);
#endif

  width  = xwdhdr->l_pixmap_width;
  height = xwdhdr->l_pixmap_height;

  image_ID = create_new_image (filename, width, height, GIMP_INDEXED,
                               GIMP_INDEXED_IMAGE, &layer_ID, &buffer);

  tile_height = gimp_tile_height ();
  data = g_malloc (tile_height * width);

  scanline = g_new (guchar, xwdhdr->l_bytes_per_line + 8);

  ncols = xwdhdr->l_colormap_entries;
  if (xwdhdr->l_ncolors < ncols)
    ncols = xwdhdr->l_ncolors;

  if (ncols < 2)
    set_bw_color_table (image_ID);
  else
    set_color_table (image_ID, xwdhdr, xwdcolmap);

  temp = (gchar *) bit2byte;

  /* Get an array for mapping 8 bits in a byte to 8 bytes */
  if (!xwdhdr->l_bitmap_bit_order)
    {
      for (j = 0; j < 256; j++)
        for (i = 0; i < 8; i++)
          *(temp++) = ((j & (1 << i)) != 0);
    }
  else
    {
      for (j = 0; j < 256; j++)
        for (i = 7; i >= 0; i--)
          *(temp++) = ((j & (1 << i)) != 0);
    }

  dest = data;
  scan_lines = 0;

  for (i = 0; i < height; i++)
    {
      if (fread (scanline, xwdhdr->l_bytes_per_line, 1, ifp) != 1)
        {
          err = 1;
          break;
        }

      /* Need to check byte order ? */
      if (xwdhdr->l_bitmap_bit_order != xwdhdr->l_byte_order)
        {
          src = scanline;
          switch (xwdhdr->l_bitmap_unit)
            {
            case 16:
              j = xwdhdr->l_bytes_per_line;
              while (j > 0)
                {
                  c1 = src[0]; c2 = src[1];
                  *(src++) = c2; *(src++) = c1;
                  j -= 2;
                }
              break;

            case 32:
              j = xwdhdr->l_bytes_per_line;
              while (j > 0)
                {
                  c1 = src[0]; c2 = src[1]; c3 = src[2]; c4 = src[3];
                  *(src++) = c4; *(src++) = c3; *(src++) = c2; *(src++) = c1;
                  j -= 4;
                }
              break;
            }
        }
      src = scanline;
      j = width;
      while (j >= 8)
        {
          pix8 = *(src++);
          memcpy (dest, bit2byte + pix8*8, 8);
          dest += 8;
          j -= 8;
        }
      if (j > 0)
        {
          pix8 = *(src++);
          memcpy (dest, bit2byte + pix8*8, j);
          dest += j;
        }

      scan_lines++;

      if ((i % 20) == 0)
        gimp_progress_update ((double)(i+1) / (double)height);

      if ((scan_lines == tile_height) || ((i+1) == height))
        {
          gegl_buffer_set (buffer, GEGL_RECTANGLE (0, i - scan_lines + 1,
                                                   width, scan_lines), 0,
                           NULL, data, GEGL_AUTO_ROWSTRIDE);

          scan_lines = 0;
          dest = data;
        }
      if (err) break;
    }

  g_free (data);
  g_free (scanline);

  if (err)
    g_message (_("EOF encountered on reading"));

  g_object_unref (buffer);

  return err ? -1 : image_ID;
}