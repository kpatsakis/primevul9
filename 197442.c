MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
        static GdkPixbufModulePattern signature[] = {
                { "GIF8", NULL, 100 },
                { NULL, NULL, 0 }
        };
	static gchar * mime_types[] = {
		"image/gif",
		NULL
	};
	static gchar * extensions[] = {
		"gif",
		NULL
	};

	info->name = "gif";
        info->signature = signature;
	info->description = N_("The GIF image format");
	info->mime_types = mime_types;
	info->extensions = extensions;
	info->flags = GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}