namespace cimg {

  // Functions to return standard streams 'stdin', 'stdout' and 'stderr'.
  // (throw a CImgIOException when macro 'cimg_use_r' is defined).
  inline FILE* _stdin(const bool throw_exception) {
#ifndef cimg_use_r
    cimg::unused(throw_exception);
    return stdin;
#else
    if (throw_exception) {
      cimg::exception_mode(0);
      throw CImgIOException("cimg::stdin(): Reference to 'stdin' stream not allowed in R mode "
                            "('cimg_use_r' is defined).");