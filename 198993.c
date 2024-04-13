cif_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{	int line_size = gdev_mem_bytes_per_scan_line((gx_device *)pdev);
        int lnum;
        byte *in = (byte *)gs_malloc(pdev->memory, line_size, 1, "cif_print_page(in)");
        char *s;
        int scanline, scanbyte;
        int length, start; /* length is the number of successive 1 bits, */
                           /* start is the set of 1 bit start position */

        if (in == 0)
                return_error(gs_error_VMerror);

        if ((s = strchr(pdev->fname, '.')) == NULL)
                length = strlen(pdev->fname) + 1;
        else
                length = s - pdev->fname;
        s = (char *)gs_malloc(pdev->memory, length+1, sizeof(char), "cif_print_page(s)");

        strncpy(s, pdev->fname, length);
        *(s + length) = '\0';
        gp_fprintf(prn_stream, "DS1 25 1;\n9 %s;\nLCP;\n", s);
        gs_free(pdev->memory, s, length+1, 1, "cif_print_page(s)");

   for (lnum = 0; lnum < pdev->height; lnum++) {
      gdev_prn_copy_scan_lines(pdev, lnum, in, line_size);
      length = 0;
      for (scanline = 0; scanline < line_size; scanline++)
#ifdef TILE			/* original, simple, inefficient algorithm */
         for (scanbyte = 0; scanbyte < 8; scanbyte++)
            if (((in[scanline] >> scanbyte) & 1) != 0)
               gp_fprintf(prn_stream, "B4 4 %d %d;\n",
                  (scanline * 8 + (7 - scanbyte)) * 4,
                  (pdev->height - lnum) * 4);
#else				/* better algorithm */
         for (scanbyte = 7; scanbyte >= 0; scanbyte--)
            /* cheap linear reduction of rectangles in lines */
            if (((in[scanline] >> scanbyte) & 1) != 0) {
               if (length == 0)
                  start = (scanline * 8 + (7 - scanbyte));
               length++;
            } else {
               if (length != 0)
                  gp_fprintf(prn_stream, "B%d 4 %d %d;\n", length * 4,
                           start * 4 + length * 2,
                           (pdev->height - lnum) * 4);
               length = 0;
            }
#endif
   }
        gp_fprintf(prn_stream, "DF;\nC1;\nE\n");
        gs_free(pdev->memory, in, line_size, 1, "cif_print_page(in)");
        return 0;
}