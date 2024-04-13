gxps_image_free (GXPSImage *image)
{
	if (G_UNLIKELY (!image))
		return;

	if (G_LIKELY (image->surface))
		cairo_surface_destroy (image->surface);

	g_slice_free (GXPSImage, image);
}