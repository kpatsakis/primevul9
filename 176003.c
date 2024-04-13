gdk_pixbuf__tiff_is_save_option_supported (const gchar *option_key)
{
        if (g_strcmp0 (option_key, "bits-per-sample") == 0 ||
            g_strcmp0 (option_key, "compression") == 0 ||
            g_strcmp0 (option_key, "icc-profile") == 0 ||
            g_strcmp0 (option_key, "x-dpi") == 0 ||
            g_strcmp0 (option_key, "y-dpi") == 0)
                return TRUE;

        return FALSE;
}