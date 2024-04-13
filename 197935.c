static int dodrawpage(fz_context *ctx, int pagenum, fz_cookie *cookie, render_details *render)
{
	fz_pixmap *pix = NULL;
	fz_bitmap *bit = NULL;
	int errors_are_fatal = 0;
	fz_irect ibounds = render->ibounds;
	fz_rect tbounds = render->tbounds;
	int total_height = ibounds.y1 - ibounds.y0;
	int start_offset = min_band_height * render->bands_rendered;
	int remaining_start = ibounds.y0 + start_offset;
	int remaining_height = ibounds.y1 - remaining_start;
	int band_height = min_band_height * render->band_height_multiple;
	int bands = (remaining_height + band_height-1) / band_height;
	fz_matrix ctm = render->ctm;
	int band;

	fz_var(pix);
	fz_var(bit);
	fz_var(errors_are_fatal);

	fz_try(ctx)
	{
		/* Set up ibounds and tbounds for a single band_height band.
		 * We will adjust ctm as we go. */
		ibounds.y1 = ibounds.y0 + band_height;
		tbounds.y1 = tbounds.y0 + band_height + 2;
		DEBUG_THREADS(("Using %d Bands\n", bands));
		ctm.f += start_offset;

		if (render->num_workers > 0)
		{
			for (band = 0; band < fz_mini(render->num_workers, bands); band++)
			{
				int band_start = start_offset + band * band_height;
				worker_t *w = &workers[band];
				w->band_start = band_start;
				w->ctm = ctm;
				w->tbounds = tbounds;
				memset(&w->cookie, 0, sizeof(fz_cookie));
				w->list = render->list;
				if (remaining_height < band_height)
					ibounds.y1 = ibounds.y0 + remaining_height;
				remaining_height -= band_height;
				w->pix = fz_new_pixmap_with_bbox(ctx, colorspace, ibounds, NULL, 0);
				w->pix->y += band * band_height;
				fz_set_pixmap_resolution(ctx, w->pix, x_resolution, y_resolution);
				DEBUG_THREADS(("Worker %d, Pre-triggering band %d\n", band, band));
				w->started = 1;
				mu_trigger_semaphore(&w->start);
			}
			pix = workers[0].pix;
		}
		else
		{
			pix = fz_new_pixmap_with_bbox(ctx, colorspace, ibounds, NULL, 0);
			fz_set_pixmap_resolution(ctx, pix, x_resolution, y_resolution);
		}

		for (band = 0; band < bands; band++)
		{
			int status;
			int band_start = start_offset + band * band_height;
			int draw_height = total_height - band_start;

			if (draw_height > band_height)
				draw_height = band_height;

			if (render->num_workers > 0)
			{
				worker_t *w = &workers[band % render->num_workers];
				DEBUG_THREADS(("Waiting for worker %d to complete band %d\n", w->num, band));
				mu_wait_semaphore(&w->stop);
				w->started = 0;
				status = w->status;
				pix = w->pix;
				bit = w->bit;
				w->bit = NULL;
				cookie->errors += w->cookie.errors;
			}
			else
				status = drawband(ctx, render->page, render->list, ctm, tbounds, cookie, band_start, pix, &bit);

			if (status != RENDER_OK)
				fz_throw(ctx, FZ_ERROR_GENERIC, "Render failed");

			render->bands_rendered += render->band_height_multiple;

			if (out)
			{
				/* If we get any errors while outputting the bands, retrying won't help. */
				errors_are_fatal = 1;
				fz_write_band(ctx, render->bander, bit ? bit->stride : pix->stride, draw_height, bit ? bit->samples : pix->samples);
				errors_are_fatal = 0;
			}
			fz_drop_bitmap(ctx, bit);
			bit = NULL;

			if (render->num_workers > 0 && band + render->num_workers < bands)
			{
				worker_t *w = &workers[band % render->num_workers];
				w->band_start = band_start;
				w->ctm = ctm;
				w->tbounds = tbounds;
				memset(&w->cookie, 0, sizeof(fz_cookie));
				DEBUG_THREADS(("Triggering worker %d for band_start= %d\n", w->num, w->band_start));
				w->started = 1;
				mu_trigger_semaphore(&w->start);
			}
			if (render->num_workers <= 0)
				pix += draw_height;
		}
	}
	fz_always(ctx)
	{
		fz_drop_bitmap(ctx, bit);
		bit = NULL;
		if (render->num_workers > 0)
		{
			int band;
			for (band = 0; band < fz_mini(render->num_workers, bands); band++)
			{
				worker_t *w = &workers[band];
				w->cookie.abort = 1;
				if (w->started)
				{
					mu_wait_semaphore(&w->stop);
					w->started = 0;
				}
				fz_drop_pixmap(ctx, w->pix);
			}
		}
		else
			fz_drop_pixmap(ctx, pix);
	}
	fz_catch(ctx)
	{
		/* Swallow error */
		if (errors_are_fatal)
			return RENDER_FATAL;
		return RENDER_RETRY;
	}
	if (cookie->errors)
		errored = 1;

	return RENDER_OK;
}