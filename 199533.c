      \param voxel_size Voxel size, to be stored in the filename.
      \param description Description, to be stored in the filename.
      \param use_bigtiff Allow to save big tiff files (>4Gb).
    **/
    const CImgList<T>& save_tiff(const char *const filename, const unsigned int compression_type=0,
                                 const float *const voxel_size=0, const char *const description=0,
                                 const bool use_bigtiff=true) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_tiff(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifndef cimg_use_tiff
      if (_width==1) _data[0].save_tiff(filename,compression_type,voxel_size,description,use_bigtiff);
      else cimglist_for(*this,l) {
          CImg<charT> nfilename(1024);
          cimg::number_filename(filename,l,6,nfilename);
          _data[l].save_tiff(nfilename,compression_type,voxel_size,description,use_bigtiff);
        }
#else
      ulongT siz = 0;
      cimglist_for(*this,l) siz+=_data[l].size();
      const bool _use_bigtiff = use_bigtiff && sizeof(siz)>=8 && siz*sizeof(T)>=1UL<<31; // No bigtiff for small images.
      TIFF *tif = TIFFOpen(filename,_use_bigtiff?"w8":"w4");
      if (tif) {
        for (unsigned int dir = 0, l = 0; l<_width; ++l) {
          const CImg<T>& img = (*this)[l];
          cimg_forZ(img,z) img._save_tiff(tif,dir++,z,compression_type,voxel_size,description);
        }
        TIFFClose(tif);
      } else
        throw CImgIOException(_cimglist_instance
                              "save_tiff(): Failed to open stream for file '%s'.",
                              cimglist_instance,