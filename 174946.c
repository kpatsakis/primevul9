ReadImage (FILE                  *fd,
           gint                   width,
           gint                   height,
           guchar                 cmap[256][3],
           gint                   ncols,
           gint                   bpp,
           gint                   compression,
           gint                   rowbytes,
           gboolean               grey,
           const Bitmap_Channel  *masks,
           GError               **error)
{
  guchar             v, n;
  GimpPixelRgn       pixel_rgn;
  gint               xpos = 0;
  gint               ypos = 0;
  gint32             image;
  gint32             layer;
  GimpDrawable      *drawable;
  guchar            *dest, *temp, *buffer;
  guchar             gimp_cmap[768];
  gushort            rgb;
  glong              rowstride, channels;
  gint               i, i_max, j, cur_progress, max_progress;
  gint               total_bytes_read;
  GimpImageBaseType  base_type;
  GimpImageType      image_type;
  guint32            px32;

  if (! (compression == BI_RGB ||
      (bpp == 8 && compression == BI_RLE8) ||
      (bpp == 4 && compression == BI_RLE4) ||
      (bpp == 16 && compression == BI_BITFIELDS) ||
      (bpp == 32 && compression == BI_BITFIELDS)))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   "%s",
                   _("Unrecognized or invalid BMP compression format."));
      return -1;
    }

  /* Make a new image in GIMP */

  switch (bpp)
    {
    case 32:
    case 24:
    case 16:
      base_type = GIMP_RGB;
      if (masks[3].mask != 0)
      {
         image_type = GIMP_RGBA_IMAGE;
         channels = 4;
      }
      else
      {
         image_type = GIMP_RGB_IMAGE;
         channels = 3;
      }
      break;

    case 8:
    case 4:
    case 1:
      if (grey)
        {
          base_type = GIMP_GRAY;
          image_type = GIMP_GRAY_IMAGE;
        }
      else
        {
          base_type = GIMP_INDEXED;
          image_type = GIMP_INDEXED_IMAGE;
        }

      channels = 1;
      break;

    default:
      g_message (_("Unsupported or invalid bitdepth."));
      return -1;
    }

  if ((width < 0) || (width > GIMP_MAX_IMAGE_SIZE))
    {
      g_message (_("Unsupported or invalid image width: %d"), width);
      return -1;
    }

  if ((height < 0) || (height > GIMP_MAX_IMAGE_SIZE))
    {
      g_message (_("Unsupported or invalid image height: %d"), height);
      return -1;
    }

  image = gimp_image_new (width, height, base_type);
  layer = gimp_layer_new (image, _("Background"),
                          width, height,
                          image_type, 100, GIMP_NORMAL_MODE);

  gimp_image_set_filename (image, filename);

  gimp_image_add_layer (image, layer, 0);
  drawable = gimp_drawable_get (layer);

  /* use g_malloc0 to initialize the dest buffer so that unspecified
     pixels in RLE bitmaps show up as the zeroth element in the palette.
  */
  dest      = g_malloc0 (drawable->width * drawable->height * channels);
  buffer    = g_malloc (rowbytes);
  rowstride = drawable->width * channels;

  ypos = height - 1;  /* Bitmaps begin in the lower left corner */
  cur_progress = 0;
  max_progress = height;

  switch (bpp)
    {
    case 32:
      {
        while (ReadOK (fd, buffer, rowbytes))
          {
            temp = dest + (ypos * rowstride);
            for (xpos= 0; xpos < width; ++xpos)
              {
                px32 = ToL(&buffer[xpos*4]);
                *(temp++)= (guchar)((px32 & masks[0].mask) >> masks[0].shiftin);
                *(temp++)= (guchar)((px32 & masks[1].mask) >> masks[1].shiftin);
                *(temp++)= (guchar)((px32 & masks[2].mask) >> masks[2].shiftin);
                if (channels > 3)
                  *(temp++)= (guchar)((px32 & masks[3].mask) >> masks[3].shiftin);
              }
            if (ypos == 0)
              break;
            --ypos; /* next line */
            cur_progress++;
            if ((cur_progress % 5) == 0)
              gimp_progress_update ((gdouble) cur_progress /
                                    (gdouble) max_progress);
          }

	if (channels == 4)
	  {
	    gboolean  has_alpha = FALSE;

	    /* at least one pixel should have nonzero alpha */
	    for (ypos = 0; ypos < height; ypos++)
	      {
		temp = dest + (ypos * rowstride);
		for (xpos = 0; xpos < width; xpos++)
		  {
		    if (temp[3])
		      {
			has_alpha = TRUE;
			break;
		      }
		    temp += 4;
		  }
		if (has_alpha)
		  break;
	      }

	    /* workaround unwanted behaviour when all alpha pixels are zero */
	    if (!has_alpha)
	      {
		for (ypos = 0; ypos < height; ypos++)
		  {
		    temp = dest + (ypos * rowstride);
		    for (xpos = 0; xpos < width; xpos++)
		      {
			temp[3] = 255;
			temp += 4;
		      }
		  }
	      }
	  }
      }
      break;

    case 24:
      {
        while (ReadOK (fd, buffer, rowbytes))
          {
            temp = dest + (ypos * rowstride);
            for (xpos= 0; xpos < width; ++xpos)
              {
                *(temp++)= buffer[xpos * 3 + 2];
                *(temp++)= buffer[xpos * 3 + 1];
                *(temp++)= buffer[xpos * 3];
              }
            if (ypos == 0)
              break;
            --ypos; /* next line */
            cur_progress++;
            if ((cur_progress % 5) == 0)
              gimp_progress_update ((gdouble) cur_progress /
                                    (gdouble) max_progress);
          }
      }
      break;

    case 16:
      {
        while (ReadOK (fd, buffer, rowbytes))
          {
            temp = dest + (ypos * rowstride);
            for (xpos= 0; xpos < width; ++xpos)
              {
                rgb= ToS(&buffer[xpos * 2]);
                *(temp++) = (guchar)(((rgb & masks[0].mask) >> masks[0].shiftin) * 255.0 / masks[0].max_value + 0.5);
                *(temp++) = (guchar)(((rgb & masks[1].mask) >> masks[1].shiftin) * 255.0 / masks[1].max_value + 0.5);
                *(temp++) = (guchar)(((rgb & masks[2].mask) >> masks[2].shiftin) * 255.0 / masks[2].max_value + 0.5);
                if (channels > 3)
                  *(temp++) = (guchar)(((rgb & masks[3].mask) >> masks[3].shiftin) * 255.0 / masks[3].max_value + 0.5);
              }
            if (ypos == 0)
              break;
            --ypos; /* next line */
            cur_progress++;
            if ((cur_progress % 5) == 0)
              gimp_progress_update ((gdouble) cur_progress /
                                    (gdouble) max_progress);
          }
      }
      break;

    case 8:
    case 4:
    case 1:
      {
        if (compression == 0)
          /* no compression */
          {
            while (ReadOK (fd, &v, 1))
              {
                for (i = 1; (i <= (8 / bpp)) && (xpos < width); i++, xpos++)
                  {
                    temp = dest + (ypos * rowstride) + (xpos * channels);
                    *temp=( v & ( ((1<<bpp)-1) << (8-(i*bpp)) ) ) >> (8-(i*bpp));
                    if (grey)
                      *temp = cmap[*temp][0];
                  }
                if (xpos == width)
                  {
                    fread(buffer, rowbytes - 1 - (width * bpp - 1) / 8, 1, fd);
                    if (ypos == 0)
                      break;
                    ypos--;
                    xpos = 0;

                    cur_progress++;
                    if ((cur_progress % 5) == 0)
                      gimp_progress_update ((gdouble) cur_progress /
                                            (gdouble) max_progress);
                  }
                if (ypos < 0)
                  break;
              }
            break;
          }
        else
          {
            /* compressed image (either RLE8 or RLE4) */
            while (ypos >= 0 && xpos <= width)
              {
                if (!ReadOK (fd, buffer, 2))
                  {
                    g_message (_("The bitmap ends unexpectedly."));
                    break;
                  }

                if ((guchar) buffer[0] != 0)
                  /* Count + Color - record */
                  {
                    /* encoded mode run -
                         buffer[0] == run_length
                         buffer[1] == pixel data
                    */
                    for (j = 0;
                         ((guchar) j < (guchar) buffer[0]) && (xpos < width);)
                      {
#ifdef DEBUG2
                        printf("%u %u | ",xpos,width);
#endif
                        for (i = 1;
                             ((i <= (8 / bpp)) &&
                              (xpos < width) &&
                              ((guchar) j < (unsigned char) buffer[0]));
                             i++, xpos++, j++)
                          {
                            temp = dest + (ypos * rowstride) + (xpos * channels);
                            *temp = (buffer[1] &
                                     (((1<<bpp)-1) << (8 - (i * bpp)))) >> (8 - (i * bpp));
                            if (grey)
                              *temp = cmap[*temp][0];
                          }
                      }
                  }
                if (((guchar) buffer[0] == 0) && ((guchar) buffer[1] > 2))
                  /* uncompressed record */
                  {
                    n = buffer[1];
                    total_bytes_read = 0;
                    for (j = 0; j < n; j += (8 / bpp))
                      {
                        /* read the next byte in the record */
                        if (!ReadOK (fd, &v, 1))
                          {
                            g_message (_("The bitmap ends unexpectedly."));
                            break;
                          }
                        total_bytes_read++;

                        /* read all pixels from that byte */
                        i_max = 8 / bpp;
                        if (n - j < i_max)
                          {
                            i_max = n - j;
                          }

                        i = 1;
                        while ((i <= i_max) && (xpos < width))
                          {
                            temp =
                              dest + (ypos * rowstride) + (xpos * channels);
                            *temp = (v >> (8-(i*bpp))) & ((1<<bpp)-1);
                            if (grey)
                              *temp = cmap[*temp][0];
                            i++;
                            xpos++;
                          }
                      }

                    /* absolute mode runs are padded to 16-bit alignment */
                    if (total_bytes_read % 2)
                      fread(&v, 1, 1, fd);
                  }
                if (((guchar) buffer[0] == 0) && ((guchar) buffer[1]==0))
                  /* Line end */
                  {
                    ypos--;
                    xpos = 0;

                    cur_progress++;
                    if ((cur_progress % 5) == 0)
                      gimp_progress_update ((gdouble) cur_progress /
                                            (gdouble)  max_progress);
                  }
                if (((guchar) buffer[0]==0) && ((guchar) buffer[1]==1))
                  /* Bitmap end */
                  {
                    break;
                  }
                if (((guchar) buffer[0]==0) && ((guchar) buffer[1]==2))
                  /* Deltarecord */
                  {
                    if (!ReadOK (fd, buffer, 2))
                      {
                        g_message (_("The bitmap ends unexpectedly."));
                        break;
                      }
                    xpos += (guchar) buffer[0];
                    ypos -= (guchar) buffer[1];
                  }
              }
            break;
          }
      }
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  fclose (fd);
  if (bpp <= 8)
    for (i = 0, j = 0; i < ncols; i++)
      {
        gimp_cmap[j++] = cmap[i][0];
        gimp_cmap[j++] = cmap[i][1];
        gimp_cmap[j++] = cmap[i][2];
      }

  gimp_progress_update (1);

  gimp_pixel_rgn_init (&pixel_rgn, drawable,
                       0, 0, drawable->width, drawable->height, TRUE, FALSE);
  gimp_pixel_rgn_set_rect (&pixel_rgn, dest,
                           0, 0, drawable->width, drawable->height);

  if ((!grey) && (bpp<= 8))
    gimp_image_set_colormap (image, gimp_cmap, ncols);

  gimp_drawable_flush (drawable);
  gimp_drawable_detach (drawable);
  g_free (dest);

  return image;
}