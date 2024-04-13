parse_switches (j_compress_ptr cinfo, int argc, char **argv,
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
  boolean force_baseline;
  boolean simple_progressive;
  char *qualityarg = NULL;      /* saves -quality parm if any */
  char *qtablefile = NULL;      /* saves -qtables filename if any */
  char *qslotsarg = NULL;       /* saves -qslots parm if any */
  char *samplearg = NULL;       /* saves -sample parm if any */
  char *scansarg = NULL;        /* saves -scans parm if any */

  /* Set up default JPEG parameters. */

  force_baseline = FALSE;       /* by default, allow 16-bit quantizers */
  simple_progressive = FALSE;
  is_targa = FALSE;
  icc_filename = NULL;
  outfilename = NULL;
  memdst = FALSE;
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

    if (keymatch(arg, "arithmetic", 1)) {
      /* Use arithmetic coding. */
#ifdef C_ARITH_CODING_SUPPORTED
      cinfo->arith_code = TRUE;
#else
      fprintf(stderr, "%s: sorry, arithmetic coding not supported\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "baseline", 1)) {
      /* Force baseline-compatible output (8-bit quantizer values). */
      force_baseline = TRUE;

    } else if (keymatch(arg, "dct", 2)) {
      /* Select DCT algorithm. */
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

    } else if (keymatch(arg, "grayscale", 2) || keymatch(arg, "greyscale",2)) {
      /* Force a monochrome JPEG file to be generated. */
      jpeg_set_colorspace(cinfo, JCS_GRAYSCALE);

    } else if (keymatch(arg, "rgb", 3)) {
      /* Force an RGB JPEG file to be generated. */
      jpeg_set_colorspace(cinfo, JCS_RGB);

    } else if (keymatch(arg, "icc", 1)) {
      /* Set ICC filename. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      icc_filename = argv[argn];

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

    } else if (keymatch(arg, "optimize", 1) || keymatch(arg, "optimise", 1)) {
      /* Enable entropy parm optimization. */
#ifdef ENTROPY_OPT_SUPPORTED
      cinfo->optimize_coding = TRUE;
#else
      fprintf(stderr, "%s: sorry, entropy optimization was not compiled in\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "outfile", 4)) {
      /* Set output file name. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      outfilename = argv[argn]; /* save it away for later use */

    } else if (keymatch(arg, "progressive", 1)) {
      /* Select simple progressive mode. */
#ifdef C_PROGRESSIVE_SUPPORTED
      simple_progressive = TRUE;
      /* We must postpone execution until num_components is known. */
#else
      fprintf(stderr, "%s: sorry, progressive output was not compiled in\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "memdst", 2)) {
      /* Use in-memory destination manager */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
      memdst = TRUE;
#else
      fprintf(stderr, "%s: sorry, in-memory destination manager was not compiled in\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "quality", 1)) {
      /* Quality ratings (quantization table scaling factors). */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      qualityarg = argv[argn];

    } else if (keymatch(arg, "qslots", 2)) {
      /* Quantization table slot numbers. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      qslotsarg = argv[argn];
      /* Must delay setting qslots until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default quant table numbers.
       */

    } else if (keymatch(arg, "qtables", 2)) {
      /* Quantization tables fetched from file. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      qtablefile = argv[argn];
      /* We postpone actually reading the file in case -quality comes later. */

    } else if (keymatch(arg, "restart", 1)) {
      /* Restart interval in MCU rows (or in MCUs with 'b'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
        usage();
      if (lval < 0 || lval > 65535L)
        usage();
      if (ch == 'b' || ch == 'B') {
        cinfo->restart_interval = (unsigned int) lval;
        cinfo->restart_in_rows = 0; /* else prior '-restart n' overrides me */
      } else {
        cinfo->restart_in_rows = (int) lval;
        /* restart_interval will be computed during startup */
      }

    } else if (keymatch(arg, "sample", 2)) {
      /* Set sampling factors. */
      if (++argn >= argc)       /* advance to next argument */
        usage();
      samplearg = argv[argn];
      /* Must delay setting sample factors until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default sampling factors.
       */

    } else if (keymatch(arg, "scans", 4)) {
      /* Set scan script. */
#ifdef C_MULTISCAN_FILES_SUPPORTED
      if (++argn >= argc)       /* advance to next argument */
        usage();
      scansarg = argv[argn];
      /* We must postpone reading the file in case -progressive appears. */
#else
      fprintf(stderr, "%s: sorry, multi-scan output was not compiled in\n",
              progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "smooth", 2)) {
      /* Set input smoothing factor. */
      int val;

      if (++argn >= argc)       /* advance to next argument */
        usage();
      if (sscanf(argv[argn], "%d", &val) != 1)
        usage();
      if (val < 0 || val > 100)
        usage();
      cinfo->smoothing_factor = val;

    } else if (keymatch(arg, "targa", 1)) {
      /* Input file is Targa format. */
      is_targa = TRUE;

    } else {
      usage();                  /* bogus switch */
    }
  }

  /* Post-switch-scanning cleanup */

  if (for_real) {

    /* Set quantization tables for selected quality. */
    /* Some or all may be overridden if -qtables is present. */
    if (qualityarg != NULL)     /* process -quality if it was present */
      if (! set_quality_ratings(cinfo, qualityarg, force_baseline))
        usage();

    if (qtablefile != NULL)     /* process -qtables if it was present */
      if (! read_quant_tables(cinfo, qtablefile, force_baseline))
        usage();

    if (qslotsarg != NULL)      /* process -qslots if it was present */
      if (! set_quant_slots(cinfo, qslotsarg))
        usage();

    if (samplearg != NULL)      /* process -sample if it was present */
      if (! set_sample_factors(cinfo, samplearg))
        usage();

#ifdef C_PROGRESSIVE_SUPPORTED
    if (simple_progressive)     /* process -progressive; -scans can override */
      jpeg_simple_progression(cinfo);
#endif

#ifdef C_MULTISCAN_FILES_SUPPORTED
    if (scansarg != NULL)       /* process -scans if it was present */
      if (! read_scan_script(cinfo, scansarg))
        usage();
#endif
  }

  return argn;                  /* return index of next arg (file name) */
}