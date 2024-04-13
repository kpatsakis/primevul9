int main(int argc, char **argv)
{
	jas_image_t *image;
	cmdopts_t *cmdopts;
	jas_stream_t *in;
	jas_stream_t *out;
	jas_tmr_t dectmr;
	jas_tmr_t enctmr;
	double dectime;
	double enctime;
	int_fast16_t numcmpts;
	int i;

	/* Determine the base name of this command. */
	if ((cmdname = strrchr(argv[0], '/'))) {
		++cmdname;
	} else {
		cmdname = argv[0];
	}

	/* Parse the command line options. */
	if (!(cmdopts = cmdopts_parse(argc, argv))) {
		fprintf(stderr, "error: cannot parse command line\n");
		exit(EXIT_FAILURE);
	}

	if (cmdopts->version) {
		printf("%s\n", JAS_VERSION);
		fprintf(stderr, "libjasper %s\n", jas_getversion());
		exit(EXIT_SUCCESS);
	}

#if defined(JAS_USE_JAS_INIT)
	if (jas_init()) {
		fprintf(stderr, "cannot initialize JasPer library\n");
		exit(EXIT_FAILURE);
	}
	jas_set_max_mem_usage(cmdopts->max_mem);
	jas_setdbglevel(cmdopts->debug);
	atexit(cleanup);
#else
	jas_conf_clear();
	static jas_std_allocator_t allocator;
	jas_std_allocator_init(&allocator);
	jas_conf_set_allocator(&allocator.base);
	jas_conf_set_debug_level(cmdopts->debug);
	jas_conf_set_max_mem_usage(cmdopts->max_mem);
	if (cmdopts->verbose < 0) {
		jas_conf_set_vlogmsgf(jas_vlogmsgf_discard);
	}
	if (jas_init_library()) {
		fprintf(stderr, "cannot initialize JasPer library\n");
		exit(EXIT_FAILURE);
	}
	if (jas_init_thread()) {
		fprintf(stderr, "cannot initialize thread\n");
		exit(EXIT_FAILURE);
	}
	atexit(cleanup);
#endif

	if (cmdopts->enable_all_formats || cmdopts->enable_format) {
		for (i = 0; i < jas_image_getnumfmts(); ++i) {
			const jas_image_fmtinfo_t *fmtinfo = jas_image_getfmtbyind(i);
			if (cmdopts->enable_all_formats || !strcmp(fmtinfo->name,
			  cmdopts->enable_format)) {
				jas_image_setfmtenable(i, 1);
			}
		}
	}

	if (cmdopts->help) {
		cmdusage();
	}

	if (cmdopts->list_codecs) {
		size_t num_formats = jas_image_getnumfmts();
		for (i = 0; i < num_formats; ++i) {
			const jas_image_fmtinfo_t *fmt;
			fmt = jas_image_getfmtbyind(i);
			if (cmdopts->list_codecs_all || fmt->enabled) {
				printf("%s\n", fmt->name);
			}
		}
		exit(EXIT_SUCCESS);
	}

	if (cmdopts->infmt_str) {
		if ((cmdopts->infmt = jas_image_strtofmt(cmdopts->infmt_str)) < 0) {
			fprintf(stderr, "warning: ignoring invalid input format %s\n",
			  cmdopts->infmt_str);
			cmdopts->infmt = -1;
		}
	} else {
		cmdopts->infmt = -1;
	}
	if (cmdopts->outfmt_str) {
		if ((cmdopts->outfmt = jas_image_strtofmt(cmdopts->outfmt_str)) < 0) {
			fprintf(stderr, "error: invalid output format %s\n",
			cmdopts->outfmt_str);
			exit(EXIT_FAILURE);
		}
	} else {
		if (cmdopts->outfmt < 0 && cmdopts->outfile) {
			if ((cmdopts->outfmt = jas_image_fmtfromname(cmdopts->outfile)) <
			  0) {
				cmdopts->outfmt = -1;
			}
		}
	}
	if (cmdopts->outfmt < 0) {
		fprintf(stderr, "error: cannot determine output image format\n");
		exit(EXIT_FAILURE);
	}

	if (cmdopts->verbose > 0) {
		cmdinfo();
	}

	/* Open the input image file. */
	if (cmdopts->infile) {
		/* The input image is to be read from a file. */
		if (!(in = jas_stream_fopen(cmdopts->infile, "rb"))) {
			fprintf(stderr, "error: cannot open input image file %s\n",
			  cmdopts->infile);
			exit(EXIT_FAILURE);
		}
	} else {
		/* The input image is to be read from standard input. */
		if (!(in = jas_stream_fdopen(0, "rb"))) {
			fprintf(stderr, "error: cannot open standard input\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Open the output image file. */
	if (cmdopts->outfile) {
		/* The output image is to be written to a file. */
		if (!(out = jas_stream_fopen(cmdopts->outfile, "w+b"))) {
			fprintf(stderr, "error: cannot open output image file %s\n",
			  cmdopts->outfile);
			exit(EXIT_FAILURE);
		}
	} else {
		/* The output image is to be written to standard output. */
		if (!(out = jas_stream_fdopen(1, "w+b"))) {
			fprintf(stderr, "error: cannot open standard output\n");
			exit(EXIT_FAILURE);
		}
	}

	if (cmdopts->infmt < 0) {
		if ((cmdopts->infmt = jas_image_getfmt(in)) < 0) {
			fprintf(stderr, "error: input image has unknown format\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Get the input image data. */
	jas_tmr_start(&dectmr);
	if (!(image = jas_image_decode(in, cmdopts->infmt, cmdopts->inopts))) {
		fprintf(stderr, "error: cannot load image data\n");
		exit(EXIT_FAILURE);
	}
	jas_tmr_stop(&dectmr);
	dectime = jas_tmr_get(&dectmr);

	/* If requested, throw away all of the components except one.
	  Why might this be desirable?  It is a hack, really.
	  None of the image formats other than the JPEG-2000 ones support
	  images with two, four, five, or more components.  This hack
	  allows such images to be decoded with the non-JPEG-2000 decoders,
	  one component at a time. */
	numcmpts = jas_image_numcmpts(image);
	if (cmdopts->cmptno >= 0 && cmdopts->cmptno < numcmpts) {
		for (i = numcmpts - 1; i >= 0; --i) {
			if (i != cmdopts->cmptno) {
				jas_image_delcmpt(image, i);
			}
		}
	}

	if (cmdopts->srgb) {
		jas_image_t *newimage;
		jas_cmprof_t *outprof;
		if (cmdopts->verbose >= 1) {
			fprintf(stderr, "forcing conversion to sRGB\n");
		}
		if (!(outprof = jas_cmprof_createfromclrspc(JAS_CLRSPC_SRGB))) {
			fprintf(stderr, "cannot create sRGB profile\n");
			exit(EXIT_FAILURE);
		}
		if (!(newimage = jas_image_chclrspc(image, outprof,
		  JAS_CMXFORM_INTENT_PER))) {
			fprintf(stderr, "cannot convert to sRGB\n");
			exit(EXIT_FAILURE);
		}
		jas_image_destroy(image);
		jas_cmprof_destroy(outprof);
		image = newimage;
	}

	/* Generate the output image data. */
	jas_tmr_start(&enctmr);
	if (jas_image_encode(image, out, cmdopts->outfmt, cmdopts->outopts)) {
		fprintf(stderr, "error: cannot encode image\n");
		exit(EXIT_FAILURE);
	}
	jas_stream_flush(out);
	jas_tmr_stop(&enctmr);
	enctime = jas_tmr_get(&enctmr);

	if (cmdopts->verbose > 0) {
		fprintf(stderr, "decoding time = %f\n", dectime);
		fprintf(stderr, "encoding time = %f\n", enctime);
	}

	/* If this fails, we don't care. */
	(void) jas_stream_close(in);

	/* Close the output image stream. */
	if (jas_stream_close(out)) {
		fprintf(stderr, "error: cannot close output image file\n");
		exit(EXIT_FAILURE);
	}

	cmdopts_destroy(cmdopts);
	jas_image_destroy(image);

	/* Success at last! :-) */
	return EXIT_SUCCESS;
}