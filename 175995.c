MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
        static const GdkPixbufModulePattern signature[] = {
                { "MM \x2a", "  z ", 100 },
                { "II\x2a ", "   z", 100 },
                { "II* \020   CR\002 ", "   z zzz   z", 0 },
                { NULL, NULL, 0 }
        };
	static const gchar *mime_types[] = {
		"image/tiff",
		NULL
	};
	static const gchar *extensions[] = {
		"tiff",
		"tif",
		NULL
	};

	info->name = "tiff";
        info->signature = (GdkPixbufModulePattern *) signature;
	info->description = NC_("image format", "TIFF");
	info->mime_types = (gchar **) mime_types;
	info->extensions = (gchar **) extensions;
	info->flags = GDK_PIXBUF_FORMAT_WRITABLE | GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}