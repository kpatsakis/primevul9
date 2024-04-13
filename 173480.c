get_gimp_image_type (const GimpImageBaseType image_base_type,
                     const gboolean          alpha)
{
  GimpImageType image_type;

  switch (image_base_type)
    {
      case GIMP_GRAY:
        image_type = (alpha) ? GIMP_GRAYA_IMAGE : GIMP_GRAY_IMAGE;
        break;

      case GIMP_INDEXED:
        image_type = (alpha) ? GIMP_INDEXEDA_IMAGE : GIMP_INDEXED_IMAGE;
        break;

      case GIMP_RGB:
        image_type = (alpha) ? GIMP_RGBA_IMAGE : GIMP_RGB_IMAGE;
        break;

      default:
        image_type = -1;
        break;
    }

  return image_type;
}