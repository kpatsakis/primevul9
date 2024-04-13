MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
	static const GdkPixbufModulePattern signature[] = {
		{ "BM", NULL, 100 },
		{ NULL, NULL, 0 }
	};
	static const gchar * mime_types[] = {
		"image/bmp",
		"image/x-bmp",
		"image/x-MS-bmp",
		NULL
	};
	static const gchar * extensions[] = {
		"bmp",
		NULL
	};

	info->name = "bmp";
	info->signature = (GdkPixbufModulePattern *) signature;
	info->description = NC_("image format", "BMP");
	info->mime_types = (gchar **) mime_types;
	info->extensions = (gchar **) extensions;
	info->flags = GDK_PIXBUF_FORMAT_WRITABLE | GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}