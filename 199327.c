    return 0;
#endif
  }

  inline FILE* _stderr(const bool throw_exception) {
#ifndef cimg_use_r
    cimg::unused(throw_exception);
    return stderr;
#else
    if (throw_exception) {
      cimg::exception_mode(0);
      throw CImgIOException("cimg::stderr(): Reference to 'stderr' stream not allowed in R mode "
                            "('cimg_use_r' is defined).");