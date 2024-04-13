int main(int argc, char **argv)
{
	char *password = "";
	fz_document *doc = NULL;
	int c;
	fz_context *ctx;
	trace_info info = { 0, 0, 0 };
	fz_alloc_context alloc_ctx = { &info, trace_malloc, trace_realloc, trace_free };
	fz_locks_context *locks = NULL;

	fz_var(doc);

	bgprint.active = 0;			/* set by -P */
	min_band_height = MIN_BAND_HEIGHT;
	max_band_memory = BAND_MEMORY;
	width = 0;
	height = 0;
	num_workers = NUM_RENDER_THREADS;
	x_resolution = X_RESOLUTION;
	y_resolution = Y_RESOLUTION;

	while ((c = fz_getopt(argc, argv, "p:o:F:R:r:w:h:fB:M:s:A:iW:H:S:T:U:XvP")) != -1)
	{
		switch (c)
		{
		default: return usage();

		case 'p': password = fz_optarg; break;

		case 'o': output = fz_optarg; break;
		case 'F': format = fz_optarg; break;

		case 'R': rotation = read_rotation(fz_optarg); break;
		case 'r': read_resolution(fz_optarg); break;
		case 'w': width = fz_atof(fz_optarg); break;
		case 'h': height = fz_atof(fz_optarg); break;
		case 'f': fit = 1; break;
		case 'B': min_band_height = atoi(fz_optarg); break;
		case 'M': max_band_memory = atoi(fz_optarg); break;

		case 'W': layout_w = fz_atof(fz_optarg); break;
		case 'H': layout_h = fz_atof(fz_optarg); break;
		case 'S': layout_em = fz_atof(fz_optarg); break;
		case 'U': layout_css = fz_optarg; break;
		case 'X': layout_use_doc_css = 0; break;

		case 's':
			if (strchr(fz_optarg, 't')) ++showtime;
			if (strchr(fz_optarg, 'm')) ++showmemory;
			break;

		case 'A':
		{
			char *sep;
			alphabits_graphics = atoi(fz_optarg);
			sep = strchr(fz_optarg, '/');
			if (sep)
				alphabits_text = atoi(sep+1);
			else
				alphabits_text = alphabits_graphics;
			break;
		}
		case 'i': ignore_errors = 1; break;

		case 'T':
#if MURASTER_THREADS != 0
			num_workers = atoi(fz_optarg); break;
#else
			fprintf(stderr, "Threads not enabled in this build\n");
			break;
#endif
		case 'P':
#if MURASTER_THREADS != 0
			bgprint.active = 1; break;
#else
			fprintf(stderr, "Threads not enabled in this build\n");
			break;
#endif
		case 'v': fprintf(stderr, "muraster version %s\n", FZ_VERSION); return 1;
		}
	}

	if (width == 0)
		width = x_resolution * PAPER_WIDTH;

	if (height == 0)
		height = y_resolution * PAPER_HEIGHT;

	if (fz_optind == argc)
		return usage();

	if (min_band_height <= 0)
	{
		fprintf(stderr, "Require a positive minimum band height\n");
		exit(1);
	}

#ifndef DISABLE_MUTHREADS
	locks = init_muraster_locks();
	if (locks == NULL)
	{
		fprintf(stderr, "cannot initialise mutexes\n");
		exit(1);
	}
#endif

	ctx = fz_new_context((showmemory == 0 ? NULL : &alloc_ctx), locks, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		exit(1);
	}

	fz_set_text_aa_level(ctx, alphabits_text);
	fz_set_graphics_aa_level(ctx, alphabits_graphics);

#ifndef DISABLE_MUTHREADS
	if (bgprint.active)
	{
		int fail = 0;
		bgprint.ctx = fz_clone_context(ctx);
		fail |= mu_create_semaphore(&bgprint.start);
		fail |= mu_create_semaphore(&bgprint.stop);
		fail |= mu_create_thread(&bgprint.thread, bgprint_worker, NULL);
		if (fail)
		{
			fprintf(stderr, "bgprint startup failed\n");
			exit(1);
		}
	}

	if (num_workers > 0)
	{
		int i;
		int fail = 0;
		workers = fz_calloc(ctx, num_workers, sizeof(*workers));
		for (i = 0; i < num_workers; i++)
		{
			workers[i].ctx = fz_clone_context(ctx);
			workers[i].num = i;
			fail |= mu_create_semaphore(&workers[i].start);
			fail |= mu_create_semaphore(&workers[i].stop);
			fail |= mu_create_thread(&workers[i].thread, worker_thread, &workers[i]);
		}
		if (fail)
		{
			fprintf(stderr, "worker startup failed\n");
			exit(1);
		}
	}
#endif /* DISABLE_MUTHREADS */

	if (layout_css)
	{
		fz_buffer *buf = fz_read_file(ctx, layout_css);
		fz_set_user_css(ctx, fz_string_from_buffer(ctx, buf));
		fz_drop_buffer(ctx, buf);
	}

	fz_set_use_document_css(ctx, layout_use_doc_css);

	output_format = suffix_table[0].format;
	output_cs = suffix_table[0].cs;
	if (format)
	{
		int i;

		for (i = 0; i < (int)nelem(suffix_table); i++)
		{
			if (!strcmp(format, suffix_table[i].suffix+1))
			{
				output_format = suffix_table[i].format;
				output_cs = suffix_table[i].cs;
				break;
			}
		}
		if (i == (int)nelem(suffix_table))
		{
			fprintf(stderr, "Unknown output format '%s'\n", format);
			exit(1);
		}
	}
	else if (output)
	{
		char *suffix = output;
		int i;

		for (i = 0; i < (int)nelem(suffix_table); i++)
		{
			char *s = strstr(suffix, suffix_table[i].suffix);

			if (s != NULL)
			{
				suffix = s+1;
				output_format = suffix_table[i].format;
				output_cs = suffix_table[i].cs;
				i = 0;
			}
		}
	}

	switch (output_cs)
	{
	case CS_GRAY:
		colorspace = fz_device_gray(ctx);
		break;
	case CS_RGB:
		colorspace = fz_device_rgb(ctx);
		break;
	case CS_CMYK:
		colorspace = fz_device_cmyk(ctx);
		break;
	}

	if (output && (output[0] != '-' || output[1] != 0) && *output != 0)
	{
		out = fz_new_output_with_path(ctx, output, 0);
	}
	else
		out = fz_stdout(ctx);

	timing.count = 0;
	timing.total = 0;
	timing.min = 1 << 30;
	timing.max = 0;
	timing.mininterp = 1 << 30;
	timing.maxinterp = 0;
	timing.minpage = 0;
	timing.maxpage = 0;
	timing.minfilename = "";
	timing.maxfilename = "";

	fz_try(ctx)
	{
		fz_register_document_handlers(ctx);

		while (fz_optind < argc)
		{
			fz_try(ctx)
			{
				filename = argv[fz_optind++];

				doc = fz_open_document(ctx, filename);

				if (fz_needs_password(ctx, doc))
				{
					if (!fz_authenticate_password(ctx, doc, password))
						fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
				}

				fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);

				if (fz_optind == argc || !fz_is_page_range(ctx, argv[fz_optind]))
					drawrange(ctx, doc, "1-N");
				if (fz_optind < argc && fz_is_page_range(ctx, argv[fz_optind]))
					drawrange(ctx, doc, argv[fz_optind++]);

				fz_drop_document(ctx, doc);
				doc = NULL;
			}
			fz_catch(ctx)
			{
				if (!ignore_errors)
					fz_rethrow(ctx);

				fz_drop_document(ctx, doc);
				doc = NULL;
				fz_warn(ctx, "ignoring error in '%s'", filename);
			}
		}
		finish_bgprint(ctx);
	}
	fz_catch(ctx)
	{
		fz_drop_document(ctx, doc);
		fprintf(stderr, "error: cannot draw '%s'\n", filename);
		errored = 1;
	}

	if (showtime && timing.count > 0)
	{
		fprintf(stderr, "total %dms / %d pages for an average of %dms\n",
			timing.total, timing.count, timing.total / timing.count);
		fprintf(stderr, "fastest page %d: %dms\n", timing.minpage, timing.min);
		fprintf(stderr, "slowest page %d: %dms\n", timing.maxpage, timing.max);
	}

#ifndef DISABLE_MUTHREADS
	if (num_workers > 0)
	{
		int i;
		for (i = 0; i < num_workers; i++)
		{
			workers[i].band_start = -1;
			mu_trigger_semaphore(&workers[i].start);
			mu_wait_semaphore(&workers[i].stop);
			mu_destroy_semaphore(&workers[i].start);
			mu_destroy_semaphore(&workers[i].stop);
			mu_destroy_thread(&workers[i].thread);
			fz_drop_context(workers[i].ctx);
		}
		fz_free(ctx, workers);
	}

	if (bgprint.active)
	{
		bgprint.pagenum = -1;
		mu_trigger_semaphore(&bgprint.start);
		mu_wait_semaphore(&bgprint.stop);
		mu_destroy_semaphore(&bgprint.start);
		mu_destroy_semaphore(&bgprint.stop);
		mu_destroy_thread(&bgprint.thread);
		fz_drop_context(bgprint.ctx);
	}
#endif /* DISABLE_MUTHREADS */

	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	out = NULL;

	fz_drop_context(ctx);
#ifndef DISABLE_MUTHREADS
	fin_muraster_locks();
#endif /* DISABLE_MUTHREADS */

	if (showmemory)
	{
		char buf[100];
		fz_snprintf(buf, sizeof buf, "Memory use total=%zu peak=%zu current=%zu", info.total, info.peak, info.current);
		fprintf(stderr, "%s\n", buf);
	}

	return (errored != 0);
}