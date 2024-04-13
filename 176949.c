create_new_image (const gchar         *filename,
                  guint                width,
                  guint                height,
                  GimpImageBaseType    type,
                  GimpImageType        gdtype,
                  gint32              *layer_ID,
                  GeglBuffer         **buffer)
{
  gint32 image_ID;

  image_ID = gimp_image_new (width, height, type);
  gimp_image_set_filename (image_ID, filename);

  *layer_ID = gimp_layer_new (image_ID, "Background", width, height,
                              gdtype, 100, GIMP_NORMAL_MODE);
  gimp_image_insert_layer (image_ID, *layer_ID, -1, 0);

  *buffer = gimp_drawable_get_buffer (*layer_ID);

  return image_ID;
}