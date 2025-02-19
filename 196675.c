main(int argc, char* argv[])
{
	uint16 photometric = 0;
	uint32 rowsperstrip = (uint32) -1;
	double resolution = -1;
	unsigned char *buf = NULL;
	tmsize_t linebytes = 0;
	uint16 spp = 1;
	uint16 bpp = 8;
	TIFF *out;
	FILE *in;
	unsigned int w, h, prec, row;
	char *infile;
	int c;
#if !HAVE_DECL_OPTARG
	extern int optind;
	extern char* optarg;
#endif
	tmsize_t scanline_size;

	if (argc < 2) {
	    fprintf(stderr, "%s: Too few arguments\n", argv[0]);
	    usage();
	}
	while ((c = getopt(argc, argv, "c:r:R:")) != -1)
		switch (c) {
		case 'c':		/* compression scheme */
			if (!processCompressOptions(optarg))
				usage();
			break;
		case 'r':		/* rows/strip */
			rowsperstrip = atoi(optarg);
			break;
		case 'R':		/* resolution */
			resolution = atof(optarg);
			break;
		case '?':
			usage();
			/*NOTREACHED*/
		}

	if (optind + 2 < argc) {
	    fprintf(stderr, "%s: Too many arguments\n", argv[0]);
	    usage();
	}

	/*
	 * If only one file is specified, read input from
	 * stdin; otherwise usage is: ppm2tiff input output.
	 */
	if (argc - optind > 1) {
		infile = argv[optind++];
		in = fopen(infile, "rb");
		if (in == NULL) {
			fprintf(stderr, "%s: Can not open.\n", infile);
			return (-1);
		}
	} else {
		infile = "<stdin>";
		in = stdin;
#if defined(HAVE_SETMODE) && defined(O_BINARY)
		setmode(fileno(stdin), O_BINARY);
#endif
	}

	if (fgetc(in) != 'P')
		BadPPM(infile);
	switch (fgetc(in)) {
		case '4':			/* it's a PBM file */
			bpp = 1;
			spp = 1;
			photometric = PHOTOMETRIC_MINISWHITE;
			break;
		case '5':			/* it's a PGM file */
			bpp = 8;
			spp = 1;
			photometric = PHOTOMETRIC_MINISBLACK;
			break;
		case '6':			/* it's a PPM file */
			bpp = 8;
			spp = 3;
			photometric = PHOTOMETRIC_RGB;
			if (compression == COMPRESSION_JPEG &&
			    jpegcolormode == JPEGCOLORMODE_RGB)
				photometric = PHOTOMETRIC_YCBCR;
			break;
		default:
			BadPPM(infile);
	}

	/* Parse header */
	while(1) {
		if (feof(in))
			BadPPM(infile);
		c = fgetc(in);
		/* Skip whitespaces (blanks, TABs, CRs, LFs) */
		if (strchr(" \t\r\n", c))
			continue;

		/* Check for comment line */
		if (c == '#') {
			do {
			    c = fgetc(in);
			} while(!(strchr("\r\n", c) || feof(in)));
			continue;
		}

		ungetc(c, in);
		break;
	}
	switch (bpp) {
	case 1:
		if (fscanf(in, " %u %u", &w, &h) != 2)
			BadPPM(infile);
		if (fgetc(in) != '\n')
			BadPPM(infile);
		break;
	case 8:
		if (fscanf(in, " %u %u %u", &w, &h, &prec) != 3)
			BadPPM(infile);
		if (fgetc(in) != '\n' || prec != 255)
			BadPPM(infile);
		break;
	}
	out = TIFFOpen(argv[optind], "w");
	if (out == NULL)
		return (-4);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint32) w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint32) h);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, spp);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bpp);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
	switch (compression) {
	case COMPRESSION_JPEG:
		TIFFSetField(out, TIFFTAG_JPEGQUALITY, quality);
		TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, jpegcolormode);
		break;
	case COMPRESSION_LZW:
	case COMPRESSION_DEFLATE:
		if (predictor != 0)
			TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
		break;
        case COMPRESSION_CCITTFAX3:
		TIFFSetField(out, TIFFTAG_GROUP3OPTIONS, g3opts);
		break;
	}
	switch (bpp) {
		case 1:
			/* if round-up overflows, result will be zero, OK */
			linebytes = (multiply_ms(spp, w) + (8 - 1)) / 8;
			if (rowsperstrip == (uint32) -1) {
				TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, h);
			} else {
				TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
				    TIFFDefaultStripSize(out, rowsperstrip));
			}
			break;
		case 8:
			linebytes = multiply_ms(spp, w);
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
			    TIFFDefaultStripSize(out, rowsperstrip));
			break;
	}
	if (linebytes == 0) {
		fprintf(stderr, "%s: scanline size overflow\n", infile);
		(void) TIFFClose(out);
		exit(-2);					
	}
	scanline_size = TIFFScanlineSize(out);
	if (scanline_size == 0) {
		/* overflow - TIFFScanlineSize already printed a message */
		(void) TIFFClose(out);
		exit(-2);					
	}
	if (scanline_size < linebytes)
		buf = (unsigned char *)_TIFFmalloc(linebytes);
	else
		buf = (unsigned char *)_TIFFmalloc(scanline_size);
	if (buf == NULL) {
		fprintf(stderr, "%s: Not enough memory\n", infile);
		(void) TIFFClose(out);
		exit(-2);
	}
	if (resolution > 0) {
		TIFFSetField(out, TIFFTAG_XRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_YRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	}
	for (row = 0; row < h; row++) {
		if (fread(buf, linebytes, 1, in) != 1) {
			fprintf(stderr, "%s: scanline %lu: Read error.\n",
			    infile, (unsigned long) row);
			break;
		}
		if (TIFFWriteScanline(out, buf, row, 0) < 0)
			break;
	}
	(void) TIFFClose(out);
	if (buf)
		_TIFFfree(buf);
	return (0);
}