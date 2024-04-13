gxps_archive_class_init (GXPSArchiveClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gxps_archive_set_property;
	object_class->finalize = gxps_archive_finalize;

	g_object_class_install_property (object_class,
					 PROP_FILE,
					 g_param_spec_object ("file",
							      "File",
							      "The archive file",
							      G_TYPE_FILE,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY));
}