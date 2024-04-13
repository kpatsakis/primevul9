read_layer_block (FILE     *f,
                  gint      image_ID,
                  guint     total_len,
                  PSPimage *ia)
{
  gint i;
  long block_start, sub_block_start, channel_start;
  gint sub_id;
  guint32 sub_init_len, sub_total_len;
  gchar *name = NULL;
  guint16 namelen;
  guchar type, opacity, blend_mode, visibility, transparency_protected;
  guchar link_group_id, mask_linked, mask_disabled;
  guint32 image_rect[4], saved_image_rect[4], mask_rect[4], saved_mask_rect[4];
  gboolean null_layer = FALSE;
  guint16 bitmap_count, channel_count;
  GimpImageType drawable_type;
  guint32 layer_ID = 0;
  GimpLayerModeEffects layer_mode;
  guint32 channel_init_len, channel_total_len;
  guint32 compressed_len, uncompressed_len;
  guint16 bitmap_type, channel_type;
  gint width, height, bytespp, offset;
  guchar **pixels, *pixel;
  GimpDrawable *drawable;
  GimpPixelRgn pixel_rgn;

  block_start = ftell (f);

  while (ftell (f) < block_start + total_len)
    {
      /* Read the layer sub-block header */
      sub_id = read_block_header (f, &sub_init_len, &sub_total_len);
      if (sub_id == -1)
        return -1;

      if (sub_id != PSP_LAYER_BLOCK)
        {
          g_message ("Invalid layer sub-block %s, should be LAYER",
                     block_name (sub_id));
          return -1;
        }

      sub_block_start = ftell (f);

      /* Read layer information chunk */
      if (psp_ver_major >= 4)
        {
          if (fseek (f, 4, SEEK_CUR) < 0
              || fread (&namelen, 2, 1, f) < 1
              || ((namelen = GUINT16_FROM_LE (namelen)) && FALSE)
              || (name = g_malloc (namelen + 1)) == NULL
              || fread (name, namelen, 1, f) < 1
              || fread (&type, 1, 1, f) < 1
              || fread (&image_rect, 16, 1, f) < 1
              || fread (&saved_image_rect, 16, 1, f) < 1
              || fread (&opacity, 1, 1, f) < 1
              || fread (&blend_mode, 1, 1, f) < 1
              || fread (&visibility, 1, 1, f) < 1
              || fread (&transparency_protected, 1, 1, f) < 1
              || fread (&link_group_id, 1, 1, f) < 1
              || fread (&mask_rect, 16, 1, f) < 1
              || fread (&saved_mask_rect, 16, 1, f) < 1
              || fread (&mask_linked, 1, 1, f) < 1
              || fread (&mask_disabled, 1, 1, f) < 1
              || fseek (f, 47, SEEK_CUR) < 0
              || fread (&bitmap_count, 2, 1, f) < 1
              || fread (&channel_count, 2, 1, f) < 1)
            {
              g_message ("Error reading layer information chunk");
              g_free (name);
              return -1;
            }

          name[namelen] = 0;
          type = PSP_LAYER_NORMAL; /* ??? */
        }
      else
        {
          name = g_malloc (257);
          name[256] = 0;

          if (fread (name, 256, 1, f) < 1
              || fread (&type, 1, 1, f) < 1
              || fread (&image_rect, 16, 1, f) < 1
              || fread (&saved_image_rect, 16, 1, f) < 1
              || fread (&opacity, 1, 1, f) < 1
              || fread (&blend_mode, 1, 1, f) < 1
              || fread (&visibility, 1, 1, f) < 1
              || fread (&transparency_protected, 1, 1, f) < 1
              || fread (&link_group_id, 1, 1, f) < 1
              || fread (&mask_rect, 16, 1, f) < 1
              || fread (&saved_mask_rect, 16, 1, f) < 1
              || fread (&mask_linked, 1, 1, f) < 1
              || fread (&mask_disabled, 1, 1, f) < 1
              || fseek (f, 43, SEEK_CUR) < 0
              || fread (&bitmap_count, 2, 1, f) < 1
              || fread (&channel_count, 2, 1, f) < 1)
            {
              g_message ("Error reading layer information chunk");
              g_free (name);
              return -1;
            }
        }

      if (type == PSP_LAYER_FLOATING_SELECTION)
        g_message ("Floating selection restored as normal layer");

      swab_rect (image_rect);
      swab_rect (saved_image_rect);
      swab_rect (mask_rect);
      swab_rect (saved_mask_rect);
      bitmap_count = GUINT16_FROM_LE (bitmap_count);
      channel_count = GUINT16_FROM_LE (channel_count);

      layer_mode = gimp_layer_mode_from_psp_blend_mode (blend_mode);
      if ((int) layer_mode == -1)
        {
          g_message ("Unsupported PSP layer blend mode %s "
                     "for layer %s, setting layer invisible",
                     blend_mode_name (blend_mode), name);
          layer_mode = GIMP_NORMAL_MODE;
          visibility = FALSE;
        }

      width = saved_image_rect[2] - saved_image_rect[0];
      height = saved_image_rect[3] - saved_image_rect[1];

      if ((width < 0) || (width > GIMP_MAX_IMAGE_SIZE)       /* w <= 2^18 */
          || (height < 0) || (height > GIMP_MAX_IMAGE_SIZE)  /* h <= 2^18 */
          || ((width / 256) * (height / 256) >= 8192))       /* w * h < 2^29 */
        {
          g_message ("Invalid layer dimensions: %dx%d", width, height);
          return -1;
        }

      IFDBG(2) g_message
        ("layer: %s %dx%d (%dx%d) @%d,%d opacity %d blend_mode %s "
         "%d bitmaps %d channels",
         name,
         image_rect[2] - image_rect[0], image_rect[3] - image_rect[1],
         width, height,
         saved_image_rect[0], saved_image_rect[1],
         opacity, blend_mode_name (blend_mode),
         bitmap_count, channel_count);

      IFDBG(2) g_message
        ("mask %dx%d (%dx%d) @%d,%d",
         mask_rect[2] - mask_rect[0],
         mask_rect[3] - mask_rect[1],
         saved_mask_rect[2] - saved_mask_rect[0],
         saved_mask_rect[3] - saved_mask_rect[1],
         saved_mask_rect[0], saved_mask_rect[1]);

      if (width == 0)
        {
          width++;
          null_layer = TRUE;
        }
      if (height == 0)
        {
          height++;
          null_layer = TRUE;
        }

      if (ia->greyscale)
        if (!null_layer && bitmap_count == 1)
          drawable_type = GIMP_GRAY_IMAGE, bytespp = 1;
        else
          drawable_type = GIMP_GRAYA_IMAGE, bytespp = 1;
      else
        if (!null_layer && bitmap_count == 1)
          drawable_type = GIMP_RGB_IMAGE, bytespp = 3;
        else
          drawable_type = GIMP_RGBA_IMAGE, bytespp = 4;

      layer_ID = gimp_layer_new (image_ID, name,
                                 width, height,
                                 drawable_type,
                                 100.0 * opacity / 255.0,
                                 layer_mode);
      if (layer_ID == -1)
        {
          g_message ("Error creating layer");
          return -1;
        }

      g_free (name);

      gimp_image_insert_layer (image_ID, layer_ID, -1, -1);

      if (saved_image_rect[0] != 0 || saved_image_rect[1] != 0)
        gimp_layer_set_offsets (layer_ID,
                                saved_image_rect[0], saved_image_rect[1]);

      if (!visibility)
        gimp_item_set_visible (layer_ID, FALSE);

      gimp_layer_set_lock_alpha (layer_ID, transparency_protected);

      if (psp_ver_major < 4)
        if (try_fseek (f, sub_block_start + sub_init_len, SEEK_SET) < 0)
          {
            return -1;
          }

      pixel = g_malloc0 (height * width * bytespp);
      if (null_layer)
        {
          pixels = NULL;
        }
      else
        {
          pixels = g_new (guchar *, height);
          for (i = 0; i < height; i++)
            pixels[i] = pixel + width * bytespp * i;
        }

      drawable = gimp_drawable_get (layer_ID);
      gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0,
                           width, height, TRUE, FALSE);

      gimp_tile_cache_size (gimp_tile_height () * width * bytespp);

      /* Read the layer channel sub-blocks */
      while (ftell (f) < sub_block_start + sub_total_len)
        {
          sub_id = read_block_header (f, &channel_init_len,
                                      &channel_total_len);
          if (sub_id == -1)
            {
              gimp_image_delete (image_ID);
              return -1;
            }

          if (sub_id != PSP_CHANNEL_BLOCK)
            {
              g_message ("Invalid layer sub-block %s, should be CHANNEL",
                         block_name (sub_id));
              return -1;
            }

          channel_start = ftell (f);

          if (psp_ver_major == 4)
            fseek (f, 4, SEEK_CUR); /* Unknown field */

          if (fread (&compressed_len, 4, 1, f) < 1
              || fread (&uncompressed_len, 4, 1, f) < 1
              || fread (&bitmap_type, 2, 1, f) < 1
              || fread (&channel_type, 2, 1, f) < 1)
            {
              g_message ("Error reading channel information chunk");
              return -1;
            }

          compressed_len = GUINT32_FROM_LE (compressed_len);
          uncompressed_len = GUINT32_FROM_LE (uncompressed_len);
          bitmap_type = GUINT16_FROM_LE (bitmap_type);
          channel_type = GUINT16_FROM_LE (channel_type);

          if (bitmap_type > PSP_DIB_USER_MASK)
            {
              g_message ("Invalid bitmap type %d in channel information chunk",
                         bitmap_type);
              return -1;
            }

          if (channel_type > PSP_CHANNEL_BLUE)
            {
              g_message ("Invalid channel type %d in channel information chunk",
                         channel_type);
              return -1;
            }

          IFDBG(2) g_message ("channel: %s %s %d (%d) bytes %d bytespp",
                              bitmap_type_name (bitmap_type),
                              channel_type_name (channel_type),
                              uncompressed_len, compressed_len,
                              bytespp);

          if (bitmap_type == PSP_DIB_TRANS_MASK)
            offset = 3;
          else
            offset = channel_type - PSP_CHANNEL_RED;

          if (psp_ver_major < 4)
            if (try_fseek (f, channel_start + channel_init_len, SEEK_SET) < 0)
              {
                return -1;
              }

          if (!null_layer)
            if (read_channel_data (f, ia, pixels, bytespp,
                                   offset, drawable, compressed_len) == -1)
              {
                return -1;
              }

          if (try_fseek (f, channel_start + channel_total_len, SEEK_SET) < 0)
            {
              return -1;
            }
        }

      gimp_pixel_rgn_set_rect (&pixel_rgn, pixel, 0, 0, width, height);

      gimp_drawable_flush (drawable);
      gimp_drawable_detach (drawable);

      g_free (pixels);
      g_free (pixel);
    }

  if (try_fseek (f, block_start + total_len, SEEK_SET) < 0)
    {
      return -1;
    }

  return layer_ID;
}