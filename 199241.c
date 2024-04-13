
    METHODDEF(void) _cimg_jpeg_error_exit(j_common_ptr cinfo) {
      _cimg_error_ptr c_err = (_cimg_error_ptr) cinfo->err;  // Return control to the setjmp point
      (*cinfo->err->format_message)(cinfo,c_err->message);
      jpeg_destroy(cinfo);  // Clean memory and temp files.
      longjmp(c_err->setjmp_buffer,1);