MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
	static const GdkPixbufModulePattern signature[] = {
		{ "\xff\xd8", NULL, 100 },
		{ NULL, NULL, 0 }
	};
	static const gchar *mime_types[] = {
		"image/jpeg",
		NULL
	};
	static const gchar *extensions[] = {
		"jpeg",
		"jpe",
		"jpg",
		NULL
	};

	info->name = "jpeg";
	info->signature = (GdkPixbufModulePattern *) signature;
	info->description = NC_("image format", "JPEG");
	info->mime_types = (gchar **) mime_types;
	info->extensions = (gchar **) extensions;
	info->flags = GDK_PIXBUF_FORMAT_WRITABLE | GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}