    **/
    const CImg<T>& save_yuv(const char *const filename,
                            const unsigned int chroma_subsampling=444,
                            const bool is_rgb=true) const {
      get_split('z').save_yuv(filename,chroma_subsampling,is_rgb);
      return *this;