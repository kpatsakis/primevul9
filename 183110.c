xwd_loader_get_frame_data (XwdLoader *loader, ChafaPixelType *pixel_type_out,
                           gint *width_out, gint *height_out, gint *rowstride_out)
{
    g_return_val_if_fail (loader != NULL, NULL);

    if (pixel_type_out)
        *pixel_type_out = compute_pixel_type (loader);
    if (width_out)
        *width_out = loader->header.pixmap_width;
    if (height_out)
        *height_out = loader->header.pixmap_height;
    if (rowstride_out)
        *rowstride_out = loader->header.bytes_per_line;

    return loader->image_data;
}