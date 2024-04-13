load_xwd_f2_d32_b32 (const gchar     *filename,
                     FILE            *ifp,
                     L_XWDFILEHEADER *xwdhdr,
                     L_XWDCOLOR      *xwdcolmap)
{
  register guchar *dest, lsbyte_first;
  gint             width, height, linepad, i, j, c0, c1, c2, c3;
  gint             tile_height, scan_lines;
  L_CARD32         pixelval;
  gint             red, green, blue, alpha, ncols;
  gint             maxred, maxgreen, maxblue, maxalpha;
  gulong           redmask, greenmask, bluemask, alphamask;
  guint            redshift, greenshift, blueshift, alphashift;
  guchar           redmap[256], greenmap[256], bluemap[256], alphamap[256];
  guchar          *data;
  PIXEL_MAP        pixel_map;
  gint             err = 0;
  gint32           layer_ID, image_ID;
  GeglBuffer      *buffer;

#ifdef XWD_DEBUG
  printf ("load_xwd_f2_d32_b32 (%s)\n", filename);
#endif

  width  = xwdhdr->l_pixmap_width;
  height = xwdhdr->l_pixmap_height;

  image_ID = create_new_image (filename, width, height, GIMP_RGB,
                               GIMP_RGBA_IMAGE, &layer_ID, &buffer);

  tile_height = gimp_tile_height ();
  data = g_malloc (tile_height * width * 4);

  redmask   = xwdhdr->l_red_mask;
  greenmask = xwdhdr->l_green_mask;
  bluemask  = xwdhdr->l_blue_mask;

  if (redmask   == 0) redmask   = 0xff0000;
  if (greenmask == 0) greenmask = 0x00ff00;
  if (bluemask  == 0) bluemask  = 0x0000ff;
  alphamask = 0xffffffff & ~(redmask | greenmask | bluemask);

  /* How to shift RGB to be right aligned ? */
  /* (We rely on the the mask bits are grouped and not mixed) */
  redshift = greenshift = blueshift = alphashift = 0;

  while (((1 << redshift)   & redmask)   == 0) redshift++;
  while (((1 << greenshift) & greenmask) == 0) greenshift++;
  while (((1 << blueshift)  & bluemask)  == 0) blueshift++;
  while (((1 << alphashift) & alphamask) == 0) alphashift++;

  /* The bits_per_rgb may not be correct. Use redmask instead */

  maxred = 0; while (redmask >> (redshift + maxred)) maxred++;
  maxred = (1 << maxred) - 1;

  maxgreen = 0; while (greenmask >> (greenshift + maxgreen)) maxgreen++;
  maxgreen = (1 << maxgreen) - 1;

  maxblue = 0; while (bluemask >> (blueshift + maxblue)) maxblue++;
  maxblue = (1 << maxblue) - 1;

  maxalpha = 0; while (alphamask >> (alphashift + maxalpha)) maxalpha++;
  maxalpha = (1 << maxalpha) - 1;

  /* Set map-arrays for red, green, blue */
  for (red = 0; red <= maxred; red++)
    redmap[red] = (red * 255) / maxred;
  for (green = 0; green <= maxgreen; green++)
    greenmap[green] = (green * 255) / maxgreen;
  for (blue = 0; blue <= maxblue; blue++)
    bluemap[blue] = (blue * 255) / maxblue;
  for (alpha = 0; alpha <= maxalpha; alpha++)
    alphamap[alpha] = (alpha * 255) / maxalpha;

  ncols = xwdhdr->l_colormap_entries;
  if (xwdhdr->l_ncolors < ncols)
    ncols = xwdhdr->l_ncolors;

  set_pixelmap (ncols, xwdcolmap, &pixel_map);

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
          c2 = getc (ifp);
          c3 = getc (ifp);
          if (c3 < 0)
            {
              err = 1;
              break;
            }
          if (lsbyte_first)
            pixelval = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
          else
            pixelval = (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;

          if (get_pixelmap (pixelval, &pixel_map, dest, dest+1, dest+2))
            {
              /* FIXME: is it always transparent or encoded in an unknown way? */
              *(dest+3) = 0x00;
              dest += 4;
            }
          else
            {
              *(dest++) = redmap[(pixelval & redmask) >> redshift];
              *(dest++) = greenmap[(pixelval & greenmask) >> greenshift];
              *(dest++) = bluemap[(pixelval & bluemask) >> blueshift];
              *(dest++) = alphamap[(pixelval & alphamask) >> alphashift];
            }
        }
      scan_lines++;

      if (err)
        break;

      for (j = 0; j < linepad; j++)
        getc (ifp);

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