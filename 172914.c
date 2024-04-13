print_text_marker (j_decompress_ptr cinfo)
{
  boolean traceit = (cinfo->err->trace_level >= 1);
  long length;
  unsigned int ch;
  unsigned int lastch = 0;

  length = jpeg_getc(cinfo) << 8;
  length += jpeg_getc(cinfo);
  length -= 2;                  /* discount the length word itself */

  if (traceit) {
    if (cinfo->unread_marker == JPEG_COM)
      fprintf(stderr, "Comment, length %ld:\n", (long) length);
    else                        /* assume it is an APPn otherwise */
      fprintf(stderr, "APP%d, length %ld:\n",
              cinfo->unread_marker - JPEG_APP0, (long) length);
  }

  while (--length >= 0) {
    ch = jpeg_getc(cinfo);
    if (traceit) {
      /* Emit the character in a readable form.
       * Nonprintables are converted to \nnn form,
       * while \ is converted to \\.
       * Newlines in CR, CR/LF, or LF form will be printed as one newline.
       */
      if (ch == '\r') {
        fprintf(stderr, "\n");
      } else if (ch == '\n') {
        if (lastch != '\r')
          fprintf(stderr, "\n");
      } else if (ch == '\\') {
        fprintf(stderr, "\\\\");
      } else if (isprint(ch)) {
        putc(ch, stderr);
      } else {
        fprintf(stderr, "\\%03o", ch);
      }
      lastch = ch;
    }
  }

  if (traceit)
    fprintf(stderr, "\n");

  return TRUE;
}