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
  fprintf(stderr, "  -quality N[,...]   Compression quality (0..100; 5-95 is most useful range,\n");
  fprintf(stderr, "                     default is 75)\n");
  fprintf(stderr, "  -grayscale     Create monochrome JPEG file\n");
  fprintf(stderr, "  -rgb           Create RGB JPEG file\n");
#ifdef ENTROPY_OPT_SUPPORTED
  fprintf(stderr, "  -optimize      Optimize Huffman table (smaller file, but slow compression)\n");
#endif
#ifdef C_PROGRESSIVE_SUPPORTED
  fprintf(stderr, "  -progressive   Create progressive JPEG file\n");
#endif
#ifdef TARGA_SUPPORTED
  fprintf(stderr, "  -targa         Input file is Targa format (usually not needed)\n");
#endif
  fprintf(stderr, "Switches for advanced users:\n");
#ifdef C_ARITH_CODING_SUPPORTED
  fprintf(stderr, "  -arithmetic    Use arithmetic coding\n");
#endif
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
  fprintf(stderr, "  -icc FILE      Embed ICC profile contained in FILE\n");
  fprintf(stderr, "  -restart N     Set restart interval in rows, or in blocks with B\n");
#ifdef INPUT_SMOOTHING_SUPPORTED
  fprintf(stderr, "  -smooth N      Smooth dithered input (N=1..100 is strength)\n");
#endif
  fprintf(stderr, "  -maxmemory N   Maximum memory to use (in kbytes)\n");
  fprintf(stderr, "  -outfile name  Specify name for output file\n");
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
  fprintf(stderr, "  -memdst        Compress to memory instead of file (useful for benchmarking)\n");
#endif
  fprintf(stderr, "  -verbose  or  -debug   Emit debug output\n");
  fprintf(stderr, "  -version       Print version information and exit\n");
  fprintf(stderr, "Switches for wizards:\n");
  fprintf(stderr, "  -baseline      Force baseline quantization tables\n");
  fprintf(stderr, "  -qtables FILE  Use quantization tables given in FILE\n");
  fprintf(stderr, "  -qslots N[,...]    Set component quantization tables\n");
  fprintf(stderr, "  -sample HxV[,...]  Set component sampling factors\n");
#ifdef C_MULTISCAN_FILES_SUPPORTED
  fprintf(stderr, "  -scans FILE    Create multi-scan JPEG per script FILE\n");
#endif
  exit(EXIT_FAILURE);
}