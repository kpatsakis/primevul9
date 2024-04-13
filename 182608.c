gxps_archive_new (GFile   *filename,
		  GError **error)
{
	return g_initable_new (GXPS_TYPE_ARCHIVE,
			       NULL, error,
			       "file", filename,
			       NULL);
}