main (int argc, char **argv)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
#ifdef PROGRESS_REPORT
  struct cdjpeg_progress_mgr progress;
#endif
  int file_index;
  cjpeg_source_ptr src_mgr;
  FILE *input_file;
  FILE *icc_file;
  JOCTET *icc_profile = NULL;
  long icc_len = 0;
  FILE *output_file = NULL;
  unsigned char *outbuffer = NULL;
  unsigned long outsize = 0;
  JDIMENSION num_scanlines;

  /* On Mac, fetch a command line. */
#ifdef USE_CCOMMAND
  argc = ccommand(&argv);
#endif

  progname = argv[0];
  if (progname == NULL || progname[0] == 0)
    progname = "cjpeg";         /* in case C library doesn't provide it */

  /* Initialize the JPEG compression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Initialize JPEG parameters.
   * Much of this may be overridden later.
   * In particular, we don't yet know the input file's color space,
   * but we need to provide some value for jpeg_set_defaults() to work.
   */

  cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
  jpeg_set_defaults(&cinfo);

  /* Scan command line to find file names.
   * It is convenient to use just one switch-parsing routine, but the switch
   * values read here are ignored; we will rescan the switches after opening
   * the input file.
   */

  file_index = parse_switches(&cinfo, argc, argv, 0, FALSE);

#ifdef TWO_FILE_COMMANDLINE
  if (!memdst) {
    /* Must have either -outfile switch or explicit output file name */
    if (outfilename == NULL) {
      if (file_index != argc-2) {
        fprintf(stderr, "%s: must name one input and one output file\n",
                progname);
        usage();
      }
      outfilename = argv[file_index+1];
    } else {
      if (file_index != argc-1) {
        fprintf(stderr, "%s: must name one input and one output file\n",
                progname);
        usage();
      }
    }
  }
#else
  /* Unix style: expect zero or one file name */
  if (file_index < argc-1) {
    fprintf(stderr, "%s: only one input file\n", progname);
    usage();
  }
#endif /* TWO_FILE_COMMANDLINE */

  /* Open the input file. */
  if (file_index < argc) {
    if ((input_file = fopen(argv[file_index], READ_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, argv[file_index]);
      exit(EXIT_FAILURE);
    }
  } else {
    /* default input file is stdin */
    input_file = read_stdin();
  }

  /* Open the output file. */
  if (outfilename != NULL) {
    if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, outfilename);
      exit(EXIT_FAILURE);
    }
  } else if (!memdst) {
    /* default output file is stdout */
    output_file = write_stdout();
  }

  if (icc_filename != NULL) {
    if ((icc_file = fopen(icc_filename, READ_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, icc_filename);
      exit(EXIT_FAILURE);
    }
    if (fseek(icc_file, 0, SEEK_END) < 0 ||
        (icc_len = ftell(icc_file)) < 1 ||
        fseek(icc_file, 0, SEEK_SET) < 0) {
      fprintf(stderr, "%s: can't determine size of %s\n", progname,
              icc_filename);
      exit(EXIT_FAILURE);
    }
    if ((icc_profile = (JOCTET *)malloc(icc_len)) == NULL) {
      fprintf(stderr, "%s: can't allocate memory for ICC profile\n", progname);
      fclose(icc_file);
      exit(EXIT_FAILURE);
    }
    if (fread(icc_profile, icc_len, 1, icc_file) < 1) {
      fprintf(stderr, "%s: can't read ICC profile from %s\n", progname,
              icc_filename);
      free(icc_profile);
      fclose(icc_file);
      exit(EXIT_FAILURE);
    }
    fclose(icc_file);
  }

#ifdef PROGRESS_REPORT
  start_progress_monitor((j_common_ptr) &cinfo, &progress);
#endif

  /* Figure out the input file format, and set up to read it. */
  src_mgr = select_file_type(&cinfo, input_file);
  src_mgr->input_file = input_file;

  /* Read the input file header to obtain file size & colorspace. */
  (*src_mgr->start_input) (&cinfo, src_mgr);

  /* Now that we know input colorspace, fix colorspace-dependent defaults */
  jpeg_default_colorspace(&cinfo);

  /* Adjust default compression parameters by re-parsing the options */
  file_index = parse_switches(&cinfo, argc, argv, 0, TRUE);

  /* Specify data destination for compression */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
  if (memdst)
    jpeg_mem_dest(&cinfo, &outbuffer, &outsize);
  else
#endif
    jpeg_stdio_dest(&cinfo, output_file);

  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  if (icc_profile != NULL)
    jpeg_write_icc_profile(&cinfo, icc_profile, (unsigned int)icc_len);

  /* Process data */
  while (cinfo.next_scanline < cinfo.image_height) {
    num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
    (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
  }

  /* Finish compression and release memory */
  (*src_mgr->finish_input) (&cinfo, src_mgr);
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  /* Close files, if we opened them */
  if (input_file != stdin)
    fclose(input_file);
  if (output_file != stdout && output_file != NULL)
    fclose(output_file);

#ifdef PROGRESS_REPORT
  end_progress_monitor((j_common_ptr) &cinfo);
#endif

  if (memdst) {
    fprintf(stderr, "Compressed size:  %lu bytes\n", outsize);
    if (outbuffer != NULL)
      free(outbuffer);
  }

  if (icc_profile != NULL)
    free(icc_profile);

  /* All done. */
  exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  return 0;                     /* suppress no-return-value warnings */
}