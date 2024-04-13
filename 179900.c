gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass       *operation_class;
  GeglOperationSourceClass *source_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  source_class    = GEGL_OPERATION_SOURCE_CLASS (klass);

  source_class->process = process;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->get_cached_region = get_cached_region;

  gegl_operation_class_set_keys (operation_class,
    "name",         "gegl:ppm-load",
    "title",        _("PPM File Loader"),
    "categories",   "hidden",
    "description",  _("PPM image loader."),
    NULL);

  gegl_operation_handlers_register_loader (
    "image/x-portable-pixmap", "gegl:ppm-load");
  gegl_operation_handlers_register_loader (
    ".ppm", "gegl:ppm-load");

  gegl_operation_handlers_register_loader (
    "image/x-portable-graymap", "gegl:ppm-load");
  gegl_operation_handlers_register_loader (
    ".pgm", "gegl:ppm-load");

  gegl_operation_handlers_register_loader (
    "image/x-portable-anymap", "gegl:ppm-load");
  gegl_operation_handlers_register_loader (
    ".pnm", "gegl:ppm-load");
}