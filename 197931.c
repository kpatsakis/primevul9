static void drawpage(fz_context *ctx, fz_document *doc, int pagenum)
{
	fz_page *page;
	fz_display_list *list = NULL;
	fz_device *list_dev = NULL;
	int start;
	fz_cookie cookie = { 0 };
#if GREY_FALLBACK != 0
	fz_device *test_dev = NULL;
	int is_color = 0;
#else
	int is_color = 2;
#endif
	render_details render;
	int status;

	fz_var(list);
	fz_var(list_dev);
	fz_var(test_dev);

	do
	{
		start = (showtime ? gettime() : 0);

		page = fz_load_page(ctx, doc, pagenum - 1);

		/* Calculate Page bounds, transform etc */
		get_page_render_details(ctx, page, &render);

		/* Make the display list, and see if we need color */
		fz_try(ctx)
		{
			list = fz_new_display_list(ctx, render.bounds);
			list_dev = fz_new_list_device(ctx, list);
#if GREY_FALLBACK != 0
			test_dev = fz_new_test_device(ctx, &is_color, 0.01f, 0, list_dev);
			fz_run_page(ctx, page, test_dev, fz_identity, &cookie);
			fz_close_device(ctx, test_dev);
#else
			fz_run_page(ctx, page, list_dev, fz_identity, &cookie);
#endif
			fz_close_device(ctx, list_dev);
		}
		fz_always(ctx)
		{
#if GREY_FALLBACK != 0
			fz_drop_device(ctx, test_dev);
#endif
			fz_drop_device(ctx, list_dev);
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			list = NULL;
			/* Just continue with no list. Also, we can't do multiple
			 * threads if we have no list. */
			render.num_workers = 1;
		}
		render.list = list;

#if GREY_FALLBACK != 0
		if (list == NULL)
		{
			/* We need to know about color, but the previous test failed
			 * (presumably) due to the size of the list. Rerun direct
			 * from file. */
			fz_try(ctx)
			{
				test_dev = fz_new_test_device(ctx, &is_color, 0.01f, 0, NULL);
				fz_run_page(ctx, page, test_dev, fz_identity, &cookie);
				fz_close_device(ctx, test_dev);
			}
			fz_always(ctx)
			{
				fz_drop_device(ctx, test_dev);
			}
			fz_catch(ctx)
			{
				/* We failed. Just give up. */
				fz_drop_page(ctx, page);
				fz_rethrow(ctx);
			}
		}
#endif

#if GREY_FALLBACK == 2
		/* If we 'possibly' need color, find out if we 'really' need color. */
		if (is_color == 1)
		{
			/* We know that the device has images or shadings in
			 * colored spaces. We have been told to test exhaustively
			 * so we know whether to use color or grey rendering. */
			is_color = 0;
			fz_try(ctx)
			{
				test_dev = fz_new_test_device(ctx, &is_color, 0.01f, FZ_TEST_OPT_IMAGES | FZ_TEST_OPT_SHADINGS, NULL);
				if (list)
					fz_run_display_list(ctx, list, test_dev, &fz_identity, &fz_infinite_rect, &cookie);
				else
					fz_run_page(ctx, page, test_dev, &fz_identity, &cookie);
				fz_close_device(ctx, test_dev);
			}
			fz_always(ctx)
			{
				fz_drop_device(ctx, test_dev);
			}
			fz_catch(ctx)
			{
				fz_drop_display_list(ctx, list);
				fz_drop_page(ctx, page);
				fz_rethrow(ctx);
			}
		}
#endif

		/* Figure out banding */
		initialise_banding(ctx, &render, is_color);

		if (bgprint.active && showtime)
		{
			int end = gettime();
			start = end - start;
		}

		/* If we're not using bgprint, then no need to wait */
		if (!bgprint.active)
			break;

		/* If we are using it, then wait for it to finish. */
		status = wait_for_bgprint_to_finish();
		if (status == RENDER_OK)
		{
			/* The background bgprint completed successfully. Drop out of the loop,
			 * and carry on with our next page. */
			break;
		}

		/* The bgprint in the background failed! This might have been because
		 * we were using memory etc in the foreground. We'd better ditch
		 * everything we can and try again. */
		fz_drop_display_list(ctx, list);
		fz_drop_page(ctx, page);

		if (status == RENDER_FATAL)
		{
			/* We failed because of not being able to output. No point in retrying. */
			fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to render page");
		}
		bgprint.started = 1;
		bgprint.solo = 1;
		mu_trigger_semaphore(&bgprint.start);
		status = wait_for_bgprint_to_finish();
		if (status != 0)
		{
			/* Hard failure */
			fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to render page");
		}
		/* Loop back to reload this page */
	}
	while (1);

	if (showtime)
	{
		fprintf(stderr, "page %s %d", filename, pagenum);
	}
	if (bgprint.active)
	{
		bgprint.started = 1;
		bgprint.solo = 0;
		bgprint.render = render;
		bgprint.filename = filename;
		bgprint.pagenum = pagenum;
		bgprint.interptime = start;
		mu_trigger_semaphore(&bgprint.start);
	}
	else
	{
		if (try_render_page(ctx, pagenum, &cookie, start, 0, filename, 0, 0, &render))
		{
			/* Hard failure */
			fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to render page");
		}
	}
}