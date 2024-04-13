gxps_archive_get_resources (GXPSArchive *archive)
{
	g_return_val_if_fail (GXPS_IS_ARCHIVE (archive), NULL);

	if (archive->resources == NULL)
		archive->resources = g_object_new (GXPS_TYPE_RESOURCES,
		                                   "archive", archive,
		                                   NULL);

	return archive->resources;
}