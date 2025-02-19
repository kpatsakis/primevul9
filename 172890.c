main (int argc, char **argv)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
#ifdef PROGRESS_REPORT
  struct cdjpeg_progress_mgr progress;
#endif
  int file_index;
  djpeg_dest_ptr dest_mgr = NULL;
  FILE *input_file;
  FILE *output_file;
  unsigned char *inbuffer = NULL;
  unsigned long insize = 0;
  JDIMENSION num_scanlines;

  /* On Mac, fetch a command line. */
#ifdef USE_CCOMMAND
  argc = ccommand(&argv);
#endif

  progname = argv[0];
  if (progname == NULL || progname[0] == 0)
    progname = "djpeg";         /* in case C library doesn't provide it */

  /* Initialize the JPEG decompression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Insert custom marker processor for COM and APP12.
   * APP12 is used by some digital camera makers for textual info,
   * so we provide the ability to display it as text.
   * If you like, additional APPn marker types can be selected for display,
   * but don't try to override APP0 or APP14 this way (see libjpeg.txt).
   */
  jpeg_set_marker_processor(&cinfo, JPEG_COM, print_text_marker);
  jpeg_set_marker_processor(&cinfo, JPEG_APP0+12, print_text_marker);

  /* Scan command line to find file names. */
  /* It is convenient to use just one switch-parsing routine, but the switch
   * values read here are ignored; we will rescan the switches after opening
   * the input file.
   * (Exception: tracing level set here controls verbosity for COM markers
   * found during jpeg_read_header...)
   */

  file_index = parse_switches(&cinfo, argc, argv, 0, FALSE);

#ifdef TWO_FILE_COMMANDLINE
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
  } else {
    /* default output file is stdout */
    output_file = write_stdout();
  }

#ifdef PROGRESS_REPORT
  start_progress_monitor((j_common_ptr) &cinfo, &progress);
#endif

  /* Specify data source for decompression */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
  if (memsrc) {
    size_t nbytes;
    do {
      inbuffer = (unsigned char *)realloc(inbuffer, insize + INPUT_BUF_SIZE);
      if (inbuffer == NULL) {
        fprintf(stderr, "%s: memory allocation failure\n", progname);
        exit(EXIT_FAILURE);
      }
      nbytes = JFREAD(input_file, &inbuffer[insize], INPUT_BUF_SIZE);
      if (nbytes < INPUT_BUF_SIZE && ferror(input_file)) {
        if (file_index < argc)
          fprintf(stderr, "%s: can't read from %s\n", progname,
                  argv[file_index]);
        else
          fprintf(stderr, "%s: can't read from stdin\n", progname);
      }
      insize += (unsigned long)nbytes;
    } while (nbytes == INPUT_BUF_SIZE);
    fprintf(stderr, "Compressed size:  %lu bytes\n", insize);
    jpeg_mem_src(&cinfo, inbuffer, insize);
  } else
#endif
    jpeg_stdio_src(&cinfo, input_file);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  /* Adjust default decompression parameters by re-parsing the options */
  file_index = parse_switches(&cinfo, argc, argv, 0, TRUE);

  /* Initialize the output module now to let it override any crucial
   * option settings (for instance, GIF wants to force color quantization).
   */
  switch (requested_fmt) {
#ifdef BMP_SUPPORTED
  case FMT_BMP:
    dest_mgr = jinit_write_bmp(&cinfo, FALSE, TRUE);
    break;
  case FMT_OS2:
    dest_mgr = jinit_write_bmp(&cinfo, TRUE, TRUE);
    break;
#endif
#ifdef GIF_SUPPORTED
  case FMT_GIF:
    dest_mgr = jinit_write_gif(&cinfo);
    break;
#endif
#ifdef PPM_SUPPORTED
  case FMT_PPM:
    dest_mgr = jinit_write_ppm(&cinfo);
    break;
#endif
#ifdef RLE_SUPPORTED
  case FMT_RLE:
    dest_mgr = jinit_write_rle(&cinfo);
    break;
#endif
#ifdef TARGA_SUPPORTED
  case FMT_TARGA:
    dest_mgr = jinit_write_targa(&cinfo);
    break;
#endif
  default:
    ERREXIT(&cinfo, JERR_UNSUPPORTED_FORMAT);
    break;
  }
  dest_mgr->output_file = output_file;

  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);

  /* Skip rows */
  if (skip) {
    JDIMENSION tmp;

    /* Check for valid skip_end.  We cannot check this value until after
     * jpeg_start_decompress() is called.  Note that we have already verified
     * that skip_start <= skip_end.
     */
    if (skip_end > cinfo.output_height - 1) {
      fprintf(stderr, "%s: skip region exceeds image height %d\n", progname,
              cinfo.output_height);
      exit(EXIT_FAILURE);
    }

    /* Write output file header.  This is a hack to ensure that the destination
     * manager creates an output image of the proper size.
     */
    tmp = cinfo.output_height;
    cinfo.output_height -= (skip_end - skip_start + 1);
    (*dest_mgr->start_output) (&cinfo, dest_mgr);
    cinfo.output_height = tmp;

    /* Process data */
    while (cinfo.output_scanline < skip_start) {
      num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
                                          dest_mgr->buffer_height);
      (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    }
    jpeg_skip_scanlines(&cinfo, skip_end - skip_start + 1);
    while (cinfo.output_scanline < cinfo.output_height) {
      num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
                                          dest_mgr->buffer_height);
      (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    }

  /* Decompress a subregion */
  } else if (crop) {
    JDIMENSION tmp;

    /* Check for valid crop dimensions.  We cannot check these values until
     * after jpeg_start_decompress() is called.
     */
    if (crop_x + crop_width > cinfo.output_width ||
        crop_y + crop_height > cinfo.output_height) {
      fprintf(stderr, "%s: crop dimensions exceed image dimensions %d x %d\n",
              progname, cinfo.output_width, cinfo.output_height);
      exit(EXIT_FAILURE);
    }

    jpeg_crop_scanline(&cinfo, &crop_x, &crop_width);
    if (dest_mgr->calc_buffer_dimensions)
      (*dest_mgr->calc_buffer_dimensions) (&cinfo, dest_mgr);
    else
      ERREXIT(&cinfo, JERR_UNSUPPORTED_FORMAT);

    /* Write output file header.  This is a hack to ensure that the destination
     * manager creates an output image of the proper size.
     */
    tmp = cinfo.output_height;
    cinfo.output_height = crop_height;
    (*dest_mgr->start_output) (&cinfo, dest_mgr);
    cinfo.output_height = tmp;

    /* Process data */
    jpeg_skip_scanlines(&cinfo, crop_y);
    while (cinfo.output_scanline < crop_y + crop_height) {
      num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
                                          dest_mgr->buffer_height);
      (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    }
    jpeg_skip_scanlines(&cinfo, cinfo.output_height - crop_y - crop_height);

  /* Normal full-image decompress */
  } else {
    /* Write output file header */
    (*dest_mgr->start_output) (&cinfo, dest_mgr);

    /* Process data */
    while (cinfo.output_scanline < cinfo.output_height) {
      num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
                                          dest_mgr->buffer_height);
      (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    }
  }

#ifdef PROGRESS_REPORT
  /* Hack: count final pass as done in case finish_output does an extra pass.
   * The library won't have updated completed_passes.
   */
  progress.pub.completed_passes = progress.pub.total_passes;
#endif

  if (icc_filename != NULL) {
    FILE *icc_file;
    JOCTET *icc_profile;
    unsigned int icc_len;

    if ((icc_file = fopen(icc_filename, WRITE_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, icc_filename);
      exit(EXIT_FAILURE);
    }
    if (jpeg_read_icc_profile(&cinfo, &icc_profile, &icc_len)) {
      if (fwrite(icc_profile, icc_len, 1, icc_file) < 1) {
        fprintf(stderr, "%s: can't read ICC profile from %s\n", progname,
                icc_filename);
        free(icc_profile);
        fclose(icc_file);
        exit(EXIT_FAILURE);
      }
      free(icc_profile);
      fclose(icc_file);
    } else if (cinfo.err->msg_code != JWRN_BOGUS_ICC)
      fprintf(stderr, "%s: no ICC profile data in JPEG file\n", progname);
  }

  /* Finish decompression and release memory.
   * I must do it in this order because output module has allocated memory
   * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
   */
  (*dest_mgr->finish_output) (&cinfo, dest_mgr);
  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  /* Close files, if we opened them */
  if (input_file != stdin)
    fclose(input_file);
  if (output_file != stdout)
    fclose(output_file);

#ifdef PROGRESS_REPORT
  end_progress_monitor((j_common_ptr) &cinfo);
#endif

  if (memsrc && inbuffer != NULL)
    free(inbuffer);

  /* All done. */
  exit(jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
  return 0;                     /* suppress no-return-value warnings */
}