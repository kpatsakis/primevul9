    return 0;
#endif
  }

  inline FILE* _stdout(const bool throw_exception) {
#ifndef cimg_use_r
    cimg::unused(throw_exception);
    return stdout;
#else
    if (throw_exception) {
      cimg::exception_mode(0);
      throw CImgIOException("cimg::stdout(): Reference to 'stdout' stream not allowed in R mode "
                            "('cimg_use_r' is defined).");