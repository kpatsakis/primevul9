add_merged_image (const gint32  image_id,
                  PSDimage     *img_a,
                  FILE         *f,
                  GError      **error)
{
  PSDchannel            chn_a[MAX_CHANNELS];
  gchar                *alpha_name;
  guchar               *pixels;
  guint16               comp_mode;
  guint16               base_channels;
  guint16               extra_channels;
  guint16               total_channels;
  guint16              *rle_pack_len[MAX_CHANNELS];
  guint32               block_len;
  guint32               block_start;
  guint32               block_end;
  guint32               alpha_id;
  gint32                layer_size;
  gint32                layer_id = -1;
  gint32                channel_id = -1;
  gint32                active_layer;
  gint16                alpha_opacity;
  gint                 *lyr_lst;
  gint                  cidx;                  /* Channel index */
  gint                  rowi;                  /* Row index */
  gint                  lyr_count;
  gint                  offset;
  gint                  i;
  gboolean              alpha_visible;
  GimpDrawable         *drawable;
  GimpPixelRgn          pixel_rgn;
  GimpImageType         image_type;
  GimpRGB               alpha_rgb;

  total_channels = img_a->channels;
  extra_channels = 0;

  if ((img_a->color_mode == PSD_BITMAP ||
       img_a->color_mode == PSD_GRAYSCALE ||
       img_a->color_mode == PSD_DUOTONE ||
       img_a->color_mode == PSD_INDEXED) &&
       total_channels > 1)
    {
      extra_channels = total_channels - 1;
    }
  else if ((img_a->color_mode == PSD_RGB ||
            img_a->color_mode == PSD_LAB) &&
            total_channels > 3)
    {
      extra_channels = total_channels - 3;
    }
  else if ((img_a->color_mode == PSD_CMYK) &&
            total_channels > 4)
    {
      extra_channels = total_channels - 4;
    }
  if (img_a->transparency && extra_channels > 0)
    extra_channels--;
  base_channels = total_channels - extra_channels;

  /* ----- Read merged image & extra channel pixel data ----- */
  if (img_a->num_layers == 0
      || extra_channels > 0)
    {
      block_start = img_a->merged_image_start;
      block_len = img_a->merged_image_len;
      block_end = block_start + block_len;
      fseek (f, block_start, SEEK_SET);

      if (fread (&comp_mode, COMP_MODE_SIZE, 1, f) < 1)
        {
          psd_set_error (feof (f), errno, error);
          return -1;
        }
      comp_mode = GUINT16_FROM_BE (comp_mode);

      switch (comp_mode)
        {
          case PSD_COMP_RAW:        /* Planar raw data */
            IFDBG(3) g_debug ("Raw data length: %d", block_len);
            for (cidx = 0; cidx < total_channels; ++cidx)
              {
                chn_a[cidx].columns = img_a->columns;
                chn_a[cidx].rows = img_a->rows;
                if (read_channel_data (&chn_a[cidx], img_a->bps,
                    PSD_COMP_RAW, NULL, f, error) < 1)
                  return -1;
              }
            break;

          case PSD_COMP_RLE:        /* Packbits */
            /* Image data is stored as packed scanlines in planar order
               with all compressed length counters stored first */
            IFDBG(3) g_debug ("RLE length data: %d, RLE data block: %d",
                               total_channels * img_a->rows * 2,
                               block_len - (total_channels * img_a->rows * 2));
            for (cidx = 0; cidx < total_channels; ++cidx)
              {
                chn_a[cidx].columns = img_a->columns;
                chn_a[cidx].rows = img_a->rows;
                rle_pack_len[cidx] = g_malloc (img_a->rows * 2);
                for (rowi = 0; rowi < img_a->rows; ++rowi)
                  {
                    if (fread (&rle_pack_len[cidx][rowi], 2, 1, f) < 1)
                      {
                        psd_set_error (feof (f), errno, error);
                        return -1;
                      }
                    rle_pack_len[cidx][rowi] = GUINT16_FROM_BE (rle_pack_len[cidx][rowi]);
                  }
              }

            IFDBG(3) g_debug ("RLE decode - data");
            for (cidx = 0; cidx < total_channels; ++cidx)
              {
                if (read_channel_data (&chn_a[cidx], img_a->bps,
                    PSD_COMP_RLE, rle_pack_len[cidx], f, error) < 1)
                  return -1;
                g_free (rle_pack_len[cidx]);
              }
            break;

          case PSD_COMP_ZIP:                 /* ? */
          case PSD_COMP_ZIP_PRED:
            g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                        _("Unsupported compression mode: %d"), comp_mode);
            return -1;
            break;
        }
    }

  /* ----- Draw merged image ----- */
  if (img_a->num_layers == 0)            /* Merged image - Photoshop 2 style */
    {
      image_type = get_gimp_image_type (img_a->base_type, img_a->transparency);

      layer_size = img_a->columns * img_a->rows;
      pixels = g_malloc (layer_size * base_channels);
      for (cidx = 0; cidx < base_channels; ++cidx)
        {
          for (i = 0; i < layer_size; ++i)
            {
              pixels[(i * base_channels) + cidx] = chn_a[cidx].data[i];
            }
          g_free (chn_a[cidx].data);
        }

      /* Add background layer */
      IFDBG(2) g_debug ("Draw merged image");
      layer_id = gimp_layer_new (image_id, _("Background"),
                                 img_a->columns, img_a->rows,
                                 image_type,
                                 100, GIMP_NORMAL_MODE);
      gimp_image_add_layer (image_id, layer_id, 0);
      drawable = gimp_drawable_get (layer_id);
      gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0,
                           drawable->width, drawable->height, TRUE, FALSE);
      gimp_pixel_rgn_set_rect (&pixel_rgn, pixels,
                               0, 0, drawable->width, drawable->height);
      gimp_drawable_flush (drawable);
      gimp_drawable_detach (drawable);
      g_free (pixels);
    }
  else
    {
      /* Free merged image data for layered image */
      if (extra_channels)
        for (cidx = 0; cidx < base_channels; ++cidx)
          g_free (chn_a[cidx].data);
    }

  /* ----- Draw extra alpha channels ----- */
  if ((extra_channels                   /* Extra alpha channels */
      || img_a->transparency)           /* Transparency alpha channel */
      && image_id > -1)
    {
      IFDBG(2) g_debug ("Add extra channels");
      pixels = g_malloc(0);

      /* Get channel resource data */
      if (img_a->transparency)
        {
          offset = 1;

          /* Free "Transparency" channel name */
          if (img_a->alpha_names)
            {
              alpha_name = g_ptr_array_index (img_a->alpha_names, 0);
              if (alpha_name)
                g_free (alpha_name);
            }
        }
      else
        offset = 0;

      /* Draw channels */
      IFDBG(2) g_debug ("Number of channels: %d", extra_channels);
      for (i = 0; i < extra_channels; ++i)
        {
          /* Alpha channel name */
          alpha_name = NULL;
          alpha_visible = FALSE;
          /* Quick mask channel*/
          if (img_a->quick_mask_id)
            if (i == img_a->quick_mask_id - base_channels + offset)
              {
                /* Free "Quick Mask" channel name */
                alpha_name = g_ptr_array_index (img_a->alpha_names, i + offset);
                if (alpha_name)
                  g_free (alpha_name);
                alpha_name = g_strdup (GIMP_IMAGE_QUICK_MASK_NAME);
                alpha_visible = TRUE;
              }
          if (! alpha_name && img_a->alpha_names)
            if (offset < img_a->alpha_names->len
                && i + offset <= img_a->alpha_names->len)
              alpha_name = g_ptr_array_index (img_a->alpha_names, i + offset);
          if (! alpha_name)
            alpha_name = g_strdup (_("Extra"));

          if (offset < img_a->alpha_id_count &&
              offset + i <= img_a->alpha_id_count)
            alpha_id = img_a->alpha_id[i + offset];
          else
            alpha_id = 0;
          if (offset < img_a->alpha_display_count &&
              i + offset <= img_a->alpha_display_count)
            {
              alpha_rgb = img_a->alpha_display_info[i + offset]->gimp_color;
              alpha_opacity = img_a->alpha_display_info[i + offset]->opacity;
            }
          else
            {
              gimp_rgba_set (&alpha_rgb, 1.0, 0.0, 0.0, 1.0);
              alpha_opacity = 50;
            }

          cidx = base_channels + i;
          pixels = g_realloc (pixels, chn_a[cidx].columns * chn_a[cidx].rows);
          memcpy (pixels, chn_a[cidx].data, chn_a[cidx].columns * chn_a[cidx].rows);
          channel_id = gimp_channel_new (image_id, alpha_name,
                                         chn_a[cidx].columns, chn_a[cidx].rows,
                                         alpha_opacity, &alpha_rgb);
          gimp_image_add_channel (image_id, channel_id, 0);
          g_free (alpha_name);
          drawable = gimp_drawable_get (channel_id);
          if (alpha_id)
            gimp_drawable_set_tattoo (drawable->drawable_id, alpha_id);
          gimp_drawable_set_visible (drawable->drawable_id, alpha_visible);
          gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0,
                                drawable->width, drawable->height,
                                TRUE, FALSE);
          gimp_pixel_rgn_set_rect (&pixel_rgn, pixels,
                                   0, 0, drawable->width,
                                   drawable->height);
          gimp_drawable_flush (drawable);
          gimp_drawable_detach (drawable);
          g_free (chn_a[cidx].data);
        }

      g_free (pixels);
      if (img_a->alpha_names)
        g_ptr_array_free (img_a->alpha_names, TRUE);

      if (img_a->alpha_id)
        g_free (img_a->alpha_id);

      if (img_a->alpha_display_info)
        {
          for (cidx = 0; cidx < img_a->alpha_display_count; ++cidx)
            g_free (img_a->alpha_display_info[cidx]);
          g_free (img_a->alpha_display_info);
        }
    }

  /* Set active layer */
  lyr_lst = gimp_image_get_layers (image_id, &lyr_count);
  if (img_a->layer_state + 1 > lyr_count ||
      img_a->layer_state + 1 < 0)
    img_a->layer_state = 0;
  active_layer = lyr_lst[lyr_count - img_a->layer_state - 1];
  gimp_image_set_active_layer (image_id, active_layer);
  g_free (lyr_lst);

  /* FIXME gimp image tattoo state */

  return 0;
}