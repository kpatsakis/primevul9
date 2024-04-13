create_gimp_image (PSDimage    *img_a,
                   const gchar *filename)
{
  gint32 image_id = -1;

  switch (img_a->color_mode)
    {
      case PSD_GRAYSCALE:
      case PSD_DUOTONE:
        img_a->base_type = GIMP_GRAY;
        break;

      case PSD_BITMAP:
      case PSD_INDEXED:
        img_a->base_type = GIMP_INDEXED;
        break;

      case PSD_RGB:
        img_a->base_type = GIMP_RGB;
        break;

      default:
        /* Color mode already validated - should not be here */
        g_warning ("Invalid color mode");
        return -1;
        break;
    }

  /* Create gimp image */
  IFDBG(2) g_debug ("Create image");
  image_id = gimp_image_new (img_a->columns, img_a->rows, img_a->base_type);

  gimp_image_set_filename (image_id, filename);
  gimp_image_undo_disable (image_id);

  return image_id;
}