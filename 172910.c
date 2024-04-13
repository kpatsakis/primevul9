parse_switches (j_decompress_ptr cinfo, int argc, char **argv,
                int last_file_arg_seen, boolean for_real)
/* Parse optional switches.
 * Returns argv[] index of first file-name argument (== argc if none).
 * Any file names with indexes <= last_file_arg_seen are ignored;
 * they have presumably been processed in a previous iteration.
 * (Pass 0 for last_file_arg_seen on the first or only iteration.)
 * for_real is FALSE on the first (dummy) pass; we may skip any expensive
 * processing.
 */
{
  int argn;
  char *arg;

  /* Set up default JPEG parameters. */
  requested_fmt = DEFAULT_FMT;  /* set default output file format */
  icc_filename = NULL;
  outfilename = NULL;
  memsrc = FALSE;
  skip = FALSE;
  crop = FALSE;
  cinfo->err->trace_level = 0;

  /* Scan command line options, adjust parameters */

  for (argn = 1; argn < argc; argn++) {
    arg = argv[argn];
    if (*arg != '-') {
      /* Not a switch, must be a file name argument */
      if (argn <= last_file_arg_seen) {
        outfilename = NULL;     /* -outfile applies to just one input file */
        continue;               /* ignore this name if previously processed */
      }
      break;                    /* else done parsing switches */
    }
    arg++;                      /* advance past switch marker character */

    if (keymatch(arg, "bmp", 1)) {
      /* BMP output format. */
      requested_fmt = FMT_BMP;

    } else if (keymatch(arg, "colors", 1) || keymatch(arg, "colours", 1) ||
               keymatch(arg, "quantize", 1) || keymatch(arg, "quantise", 1)) {
      /* Do color quantization. */
      int val;

      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (sscanf(argv[argn], "%d", &val) != 1)
        usage();
      cinfo->desired_number_of_colors = val;
      cinfo->quantize_colors = TRUE;

    } else if (keymatch(arg, "dct", 2)) {
      /* Select IDCT algorithm. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (keymatch(argv[argn], "int", 1)) {
        cinfo->dct_method = JDCT_ISLOW;
      } else if (keymatch(argv[argn], "fast", 2)) {
        cinfo->dct_method = JDCT_IFAST;
      } else if (keymatch(argv[argn], "float", 2)) {
        cinfo->dct_method = JDCT_FLOAT;
      } else
        usage();

    } else if (keymatch(arg, "dither", 2)) {
      /* Select dithering algorithm. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (keymatch(argv[argn], "fs", 2)) {
        cinfo->dither_mode = JDITHER_FS;
      } else if (keymatch(argv[argn], "none", 2)) {
        cinfo->dither_mode = JDITHER_NONE;
      } else if (keymatch(argv[argn], "ordered", 2)) {
        cinfo->dither_mode = JDITHER_ORDERED;
      } else
        usage();

    } else if (keymatch(arg, "debug", 1) || keymatch(arg, "verbose", 1)) {
      /* Enable debug printouts. */
      /* On first -d, print version identification */
      static boolean printed_version = FALSE;

      if (! printed_version) {
        fprintf(stderr, "%s version %s (build %s)\n",
                PACKAGE_NAME, VERSION, BUILD);
        fprintf(stderr, "%s\n\n", JCOPYRIGHT);
        fprintf(stderr, "Emulating The Independent JPEG Group's software, version %s\n\n",
                JVERSION);
        printed_version = TRUE;
      }
      cinfo->err->trace_level++;

    } else if (keymatch(arg, "version", 4)) {
      fprintf(stderr, "%s version %s (build %s)\n",
              PACKAGE_NAME, VERSION, BUILD);
      exit(EXIT_SUCCESS);

    } else if (keymatch(arg, "fast", 1)) {
      /* Select recommended processing options for quick-and-dirty output. */
      cinfo->two_pass_quantize = FALSE;
      cinfo->dither_mode = JDITHER_ORDERED;
      if (! cinfo->quantize_colors) /* don't override an earlier -colors */
        cinfo->desired_number_of_colors = 216;
      cinfo->dct_method = JDCT_FASTEST;
      cinfo->do_fancy_upsampling = FALSE;

    } else if (keymatch(arg, "gif", 1)) {
      /* GIF output format. */
      requested_fmt = FMT_GIF;

    } else if (keymatch(arg, "grayscale", 2) || keymatch(arg, "greyscale",2)) {
      /* Force monochrome output. */
      cinfo->out_color_space = JCS_GRAYSCALE;

    } else if (keymatch(arg, "rgb", 2)) {
      /* Force RGB output. */
      cinfo->out_color_space = JCS_RGB;

    } else if (keymatch(arg, "rgb565", 2)) {
      /* Force RGB565 output. */
      cinfo->out_color_space = JCS_RGB565;

    } else if (keymatch(arg, "icc", 1)) {
      /* Set ICC filename. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      icc_filename = argv[argn];
      jpeg_save_markers(cinfo, JPEG_APP0 + 2, 0xFFFF);

    } else if (keymatch(arg, "map", 3)) {
      /* Quantize to a color map taken from an input file. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (for_real) {           /* too expensive to do twice! */
#ifdef QUANT_2PASS_SUPPORTED    /* otherwise can't quantize to supplied map */
        FILE *mapfile;

        if ((mapfile = fopen(argv[argn], READ_BINARY)) == NULL) {
          fprintf(stderr, "%s: can't open %s\n", progname, argv[argn]);
          exit(EXIT_FAILURE);
        }
        read_color_map(cinfo, mapfile);
        fclose(mapfile);
        cinfo->quantize_colors = TRUE;
#else
        ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
      }

    } else if (keymatch(arg, "maxmemory", 3)) {
      /* Maximum memory in Kb (or Mb with 'm'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
        usage();
      if (ch == 'm' || ch == 'M')
        lval *= 1000L;
      cinfo->mem->max_memory_to_use = lval * 1000L;

    } else if (keymatch(arg, "nosmooth", 3)) {
      /* Suppress fancy upsampling */
      cinfo->do_fancy_upsampling = FALSE;

    } else if (keymatch(arg, "onepass", 3)) {
      /* Use fast one-pass quantization. */
      cinfo->two_pass_quantize = FALSE;

    } else if (keymatch(arg, "os2", 3)) {
      /* BMP output format (OS/2 flavor). */
      requested_fmt = FMT_OS2;

    } else if (keymatch(arg, "outfile", 4)) {
      /* Set output file name. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      outfilename = argv[argn]; /* save it away for later use */

    } else if (keymatch(arg, "memsrc", 2)) {
      /* Use in-memory source manager */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
      memsrc = TRUE;
#else
      fprintf(stderr, "%s: sorry, in-memory source manager was not compiled in\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "pnm", 1) || keymatch(arg, "ppm", 1)) {
      /* PPM/PGM output format. */
      requested_fmt = FMT_PPM;

    } else if (keymatch(arg, "rle", 1)) {
      /* RLE output format. */
      requested_fmt = FMT_RLE;

    } else if (keymatch(arg, "scale", 2)) {
      /* Scale the output image by a fraction M/N. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (sscanf(argv[argn], "%u/%u",
                 &cinfo->scale_num, &cinfo->scale_denom) != 2)
        usage();

    } else if (keymatch(arg, "skip", 2)) {
      if (++argn >= argc)
        usage();
      if (sscanf(argv[argn], "%u,%u", &skip_start, &skip_end) != 2 ||
          skip_start > skip_end)
        usage();
      skip = TRUE;

    } else if (keymatch(arg, "crop", 2)) {
      char c;
      if (++argn >= argc)
        usage();
      if (sscanf(argv[argn], "%u%c%u+%u+%u", &crop_width, &c, &crop_height,
                 &crop_x, &crop_y) != 5 ||
          (c != 'X' && c != 'x') || crop_width < 1 || crop_height < 1)
        usage();
      crop = TRUE;

    } else if (keymatch(arg, "targa", 1)) {
      /* Targa output format. */
      requested_fmt = FMT_TARGA;

    } else {
      usage();                  /* bogus switch */
    }
  }

  return argn;                  /* return index of next arg (file name) */
}