    return dt;
  }

  // Return a temporary string describing the size of a memory buffer.
  inline const char *strbuffersize(const cimg_ulong size) {
    static CImg<char> res(256);
    cimg::mutex(5);
    if (size<1024LU) cimg_snprintf(res,res._width,"%lu byte%s",(unsigned long)size,size>1?"s":"");
    else if (size<1024*1024LU) { const float nsize = size/1024.0f; cimg_snprintf(res,res._width,"%.1f Kio",nsize); }
    else if (size<1024*1024*1024LU) {
      const float nsize = size/(1024*1024.0f); cimg_snprintf(res,res._width,"%.1f Mio",nsize);