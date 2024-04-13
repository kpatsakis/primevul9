gdk_pixbuf__jpeg_is_save_option_supported (const gchar *option_key)
{
        if (g_strcmp0 (option_key, "quality") == 0 ||
            g_strcmp0 (option_key, "icc-profile") == 0)
                return TRUE;

        return FALSE;
}