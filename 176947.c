load_xwd_f2_d16_b16 (const gchar     *filename,
                     FILE            *ifp,
                     L_XWDFILEHEADER *xwdhdr,
                     L_XWDCOLOR      *xwdcolmap)
{
  register guchar *dest, lsbyte_first;
  gint             width, height, linepad, i, j, c0, c1, ncols;
  gint             red, green, blue, redval, greenval, blueval;
  gint             maxred, maxgreen, maxblue;
  gint             tile_height, scan_lines;
  gulong           redmask, greenmask, bluemask;
  guint            redshift, greenshift, blueshift;
  gulong           maxval;
  guchar          *ColorMap, *cm, *data;
  gint             err = 0;
  gint32           layer_ID, image_ID;
  GeglBuffer      *buffer;

#ifdef XWD_DEBUG
  printf ("load_xwd_f2_d16_b16 (%s)\n", filename);
#endif

  width  = xwdhdr->l_pixmap_width;
  height = xwdhdr->l_pixmap_height;

  image_ID = create_new_image (filename, width, height, GIMP_RGB,
                               GIMP_RGB_IMAGE, &layer_ID, &buffer);

  tile_height = gimp_tile_height ();
  data = g_malloc (tile_height * width * 3);

  /* Get memory for mapping 16 bit XWD-pixel to GIMP-RGB */
  maxval = 0x10000 * 3;
  ColorMap = g_new0 (guchar, maxval);

  redmask   = xwdhdr->l_red_mask;
  greenmask = xwdhdr->l_green_mask;
  bluemask  = xwdhdr->l_blue_mask;

  /* How to shift RGB to be right aligned ? */
  /* (We rely on the the mask bits are grouped and not mixed) */
  redshift = greenshift = blueshift = 0;

  while (((1 << redshift)   & redmask)   == 0) redshift++;
  while (((1 << greenshift) & greenmask) == 0) greenshift++;
  while (((1 << blueshift)  & bluemask)  == 0) blueshift++;

  /* The bits_per_rgb may not be correct. Use redmask instead */
  maxred = 0; while (redmask >> (redshift + maxred)) maxred++;
  maxred = (1 << maxred) - 1;

  maxgreen = 0; while (greenmask >> (greenshift + maxgreen)) maxgreen++;
  maxgreen = (1 << maxgreen) - 1;

  maxblue = 0; while (bluemask >> (blueshift + maxblue)) maxblue++;
  maxblue = (1 << maxblue) - 1;

  /* Built up the array to map XWD-pixel value to GIMP-RGB */
  for (red = 0; red <= maxred; red++)
    {
      redval = (red * 255) / maxred;
      for (green = 0; green <= maxgreen; green++)
        {
          greenval = (green * 255) / maxgreen;
          for (blue = 0; blue <= maxblue; blue++)
            {
              blueval = (blue * 255) / maxblue;
              cm = ColorMap + ((red << redshift) + (green << greenshift)
                               + (blue << blueshift)) * 3;
              *(cm++) = redval;
              *(cm++) = greenval;
              *cm = blueval;
            }
        }
    }

  /* Now look what was written to the XWD-Colormap */

  ncols = xwdhdr->l_colormap_entries;
  if (xwdhdr->l_ncolors < ncols)
    ncols = xwdhdr->l_ncolors;

  for (j = 0; j < ncols; j++)
    {
      cm = ColorMap + xwdcolmap[j].l_pixel * 3;
      *(cm++) = (xwdcolmap[j].l_red >> 8);
      *(cm++) = (xwdcolmap[j].l_green >> 8);
      *cm = (xwdcolmap[j].l_blue >> 8);
    }

  /* What do we have to consume after a line has finished ? */
  linepad =   xwdhdr->l_bytes_per_line
    - (xwdhdr->l_pixmap_width*xwdhdr->l_bits_per_pixel)/8;
  if (linepad < 0) linepad = 0;

  lsbyte_first = (xwdhdr->l_byte_order == 0);

  dest = data;
  scan_lines = 0;

  for (i = 0; i < height; i++)
    {
      for (j = 0; j < width; j++)
        {
          c0 = getc (ifp);
          c1 = getc (ifp);
          if (c1 < 0)
            {
              err = 1;
              break;
            }

          if (lsbyte_first)
            c0 = c0 | (c1 << 8);
          else
            c0 = (c0 << 8) | c1;

          cm = ColorMap + c0 * 3;
          *(dest++) = *(cm++);
          *(dest++) = *(cm++);
          *(dest++) = *cm;
        }

      if (err)
        break;

      for (j = 0; j < linepad; j++)
        getc (ifp);

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
    }
  g_free (data);
  g_free (ColorMap);

  if (err)
    g_message (_("EOF encountered on reading"));

  g_object_unref (buffer);

  return err ? -1 : image_ID;
}