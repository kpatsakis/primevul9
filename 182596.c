gxps_archive_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
	GXPSArchive *archive = GXPS_ARCHIVE (object);

	switch (prop_id) {
	case PROP_FILE:
		archive->filename = g_value_dup_object (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}