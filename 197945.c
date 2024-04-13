static int drawband(fz_context *ctx, fz_page *page, fz_display_list *list, fz_matrix ctm, fz_rect tbounds, fz_cookie *cookie, int band_start, fz_pixmap *pix, fz_bitmap **bit)
{
	fz_device *dev = NULL;

	*bit = NULL;

	fz_try(ctx)
	{
		fz_clear_pixmap_with_value(ctx, pix, 255);

		dev = fz_new_draw_device(ctx, fz_identity, pix);
		if (alphabits_graphics == 0)
			fz_enable_device_hints(ctx, dev, FZ_DONT_INTERPOLATE_IMAGES);
		if (list)
			fz_run_display_list(ctx, list, dev, ctm, tbounds, cookie);
		else
			fz_run_page(ctx, page, dev, ctm, cookie);
		fz_close_device(ctx, dev);
		fz_drop_device(ctx, dev);
		dev = NULL;

		if ((output_format == OUT_PBM) || (output_format == OUT_PKM))
			*bit = fz_new_bitmap_from_pixmap_band(ctx, pix, NULL, band_start);
	}
	fz_catch(ctx)
	{
		fz_drop_device(ctx, dev);
		return RENDER_RETRY;
	}
	return RENDER_OK;
}