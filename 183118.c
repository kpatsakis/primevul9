xwd_loader_destroy (XwdLoader *loader)
{
    if (loader->mapping)
        file_mapping_destroy (loader->mapping);

    g_free (loader);
}