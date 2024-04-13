stdio_fill_input_buffer (j_decompress_ptr cinfo)
{
  stdio_src_ptr src = (stdio_src_ptr) cinfo->src;
  size_t nbytes;

  nbytes = fread (src->buffer, 1, JPEG_PROG_BUF_SIZE, src->infile);

  if (nbytes <= 0) {
#if 0
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
#endif
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}