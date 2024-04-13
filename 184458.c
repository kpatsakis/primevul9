read_general_image_attribute_block (FILE     *f,
                                    guint     init_len,
                                    guint     total_len,
                                    PSPimage *ia)
{
  gchar buf[6];
  guint64 res;
  gchar graphics_content[4];

  if (init_len < 38 || total_len < 38)
    {
      g_message ("Invalid general image attribute chunk size");
      return -1;
    }

  if (psp_ver_major >= 4)
    {
      /* TODO: This causes the chunk size to be ignored. Better verify if it is
       *       valid since it might create read offset problems with the
       *       "expansion field" (which follows after the "graphics content" and
       *       is of unkown size).
       */
      fseek (f, 4, SEEK_CUR);
    }

  if (fread (&ia->width, 4, 1, f) < 1
      || fread (&ia->height, 4, 1, f) < 1
      || fread (&res, 8, 1, f) < 1
      || fread (&ia->metric, 1, 1, f) < 1
      || fread (&ia->compression, 2, 1, f) < 1
      || fread (&ia->depth, 2, 1, f) < 1
      || fread (buf, 2+4, 1, f) < 1 /* Skip plane and colour count */
      || fread (&ia->greyscale, 1, 1, f) < 1
      || fread (buf, 4, 1, f) < 1 /* Skip total image size */
      || fread (&ia->active_layer, 4, 1, f) < 1
      || fread (&ia->layer_count, 2, 1, f) < 1
      || (psp_ver_major >= 4 && fread (graphics_content, 4, 1, f) < 1))
    {
      g_message ("Error reading general image attribute block");
      return -1;
    }
  ia->width = GUINT32_FROM_LE (ia->width);
  ia->height = GUINT32_FROM_LE (ia->height);

  res = GUINT64_FROM_LE (res);
  memcpy (&ia->resolution, &res, 8);
  if (ia->metric == PSP_METRIC_CM)
    ia->resolution /= 2.54;

  ia->compression = GUINT16_FROM_LE (ia->compression);
  if (ia->compression > PSP_COMP_LZ77)
    {
      g_message ("Unknown compression type %d", ia->compression);
      return -1;
    }

  ia->depth = GUINT16_FROM_LE (ia->depth);
  if (ia->depth != 24)
    {
      g_message ("Unsupported bit depth %d", ia->depth);
      return -1;
    }

  ia->active_layer = GUINT32_FROM_LE (ia->active_layer);
  ia->layer_count = GUINT16_FROM_LE (ia->layer_count);

  return 0;
}