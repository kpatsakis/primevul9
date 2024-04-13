     **/
    const CImg<T>& save_tiff(const char *const filename, const unsigned int compression_type=0,
                             const float *const voxel_size=0, const char *const description=0,
                             const bool use_bigtiff=true) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_tiff(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifdef cimg_use_tiff
      const bool
        _use_bigtiff = use_bigtiff && sizeof(ulongT)>=8 && size()*sizeof(T)>=1UL<<31; // No bigtiff for small images.
      TIFF *tif = TIFFOpen(filename,_use_bigtiff?"w8":"w4");
      if (tif) {
        cimg_forZ(*this,z) _save_tiff(tif,z,z,compression_type,voxel_size,description);
        TIFFClose(tif);
      } else throw CImgIOException(_cimg_instance
                                   "save_tiff(): Failed to open file '%s' for writing.",
                                   cimg_instance,
                                   filename);
      return *this;
#else
      cimg::unused(compression_type,voxel_size,description,use_bigtiff);
      return save_other(filename);
#endif