xwd_loader_new_from_mapping (FileMapping *mapping)
{
    XwdLoader *loader = NULL;
    gboolean success = FALSE;

    g_return_val_if_fail (mapping != NULL, NULL);

    loader = xwd_loader_new ();
    loader->mapping = mapping;

    if (!load_header (loader))
    {
        g_free (loader);
        return NULL;
    }

    DEBUG (dump_header (&loader->header));

    if (loader->header.pixmap_width < 1 || loader->header.pixmap_width >= (1 << 28)
        || loader->header.pixmap_height < 1 || loader->header.pixmap_height >= (1 << 28)
        || (loader->header.pixmap_width * (guint64) loader->header.pixmap_height >= (1 << 29)))
        goto out;

    success = TRUE;

out:
    if (!success)
    {
        g_free (loader);
        loader = NULL;
    }

    return loader;
}