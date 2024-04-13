usage (void)
/* complain about bad command line */
{
  fprintf(stderr, "usage: %s [switches] ", progname);
#ifdef TWO_FILE_COMMANDLINE
  fprintf(stderr, "inputfile outputfile\n");
#else
  fprintf(stderr, "[inputfile]\n");
#endif

  fprintf(stderr, "Switches (names may be abbreviated):\n");
  fprintf(stderr, "  -colors N      Reduce image to no more than N colors\n");
  fprintf(stderr, "  -fast          Fast, low-quality processing\n");
  fprintf(stderr, "  -grayscale     Force grayscale output\n");
  fprintf(stderr, "  -rgb           Force RGB output\n");
  fprintf(stderr, "  -rgb565        Force RGB565 output\n");
#ifdef IDCT_SCALING_SUPPORTED
  fprintf(stderr, "  -scale M/N     Scale output image by fraction M/N, eg, 1/8\n");
#endif
#ifdef BMP_SUPPORTED
  fprintf(stderr, "  -bmp           Select BMP output format (Windows style)%s\n",
          (DEFAULT_FMT == FMT_BMP ? " (default)" : ""));
#endif
#ifdef GIF_SUPPORTED
  fprintf(stderr, "  -gif           Select GIF output format%s\n",
          (DEFAULT_FMT == FMT_GIF ? " (default)" : ""));
#endif
#ifdef BMP_SUPPORTED
  fprintf(stderr, "  -os2           Select BMP output format (OS/2 style)%s\n",
          (DEFAULT_FMT == FMT_OS2 ? " (default)" : ""));
#endif
#ifdef PPM_SUPPORTED
  fprintf(stderr, "  -pnm           Select PBMPLUS (PPM/PGM) output format%s\n",
          (DEFAULT_FMT == FMT_PPM ? " (default)" : ""));
#endif
#ifdef RLE_SUPPORTED
  fprintf(stderr, "  -rle           Select Utah RLE output format%s\n",
          (DEFAULT_FMT == FMT_RLE ? " (default)" : ""));
#endif
#ifdef TARGA_SUPPORTED
  fprintf(stderr, "  -targa         Select Targa output format%s\n",
          (DEFAULT_FMT == FMT_TARGA ? " (default)" : ""));
#endif
  fprintf(stderr, "Switches for advanced users:\n");
#ifdef DCT_ISLOW_SUPPORTED
  fprintf(stderr, "  -dct int       Use integer DCT method%s\n",
          (JDCT_DEFAULT == JDCT_ISLOW ? " (default)" : ""));
#endif
#ifdef DCT_IFAST_SUPPORTED
  fprintf(stderr, "  -dct fast      Use fast integer DCT (less accurate)%s\n",
          (JDCT_DEFAULT == JDCT_IFAST ? " (default)" : ""));
#endif
#ifdef DCT_FLOAT_SUPPORTED
  fprintf(stderr, "  -dct float     Use floating-point DCT method%s\n",
          (JDCT_DEFAULT == JDCT_FLOAT ? " (default)" : ""));
#endif
  fprintf(stderr, "  -dither fs     Use F-S dithering (default)\n");
  fprintf(stderr, "  -dither none   Don't use dithering in quantization\n");
  fprintf(stderr, "  -dither ordered  Use ordered dither (medium speed, quality)\n");
  fprintf(stderr, "  -icc FILE      Extract ICC profile to FILE\n");
#ifdef QUANT_2PASS_SUPPORTED
  fprintf(stderr, "  -map FILE      Map to colors used in named image file\n");
#endif
  fprintf(stderr, "  -nosmooth      Don't use high-quality upsampling\n");
#ifdef QUANT_1PASS_SUPPORTED
  fprintf(stderr, "  -onepass       Use 1-pass quantization (fast, low quality)\n");
#endif
  fprintf(stderr, "  -maxmemory N   Maximum memory to use (in kbytes)\n");
  fprintf(stderr, "  -outfile name  Specify name for output file\n");
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
  fprintf(stderr, "  -memsrc        Load input file into memory before decompressing\n");
#endif

  fprintf(stderr, "  -skip Y0,Y1    Decompress all rows except those between Y0 and Y1 (inclusive)\n");
  fprintf(stderr, "  -crop WxH+X+Y  Decompress only a rectangular subregion of the image\n");
  fprintf(stderr, "                 [requires PBMPLUS (PPM/PGM), GIF, or Targa output format]\n");
  fprintf(stderr, "  -verbose  or  -debug   Emit debug output\n");
  fprintf(stderr, "  -version       Print version information and exit\n");
  exit(EXIT_FAILURE);
}