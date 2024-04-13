    **/
    const CImg<T>& save_imagemagick_external(const char *const filename, const unsigned int quality=100) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_imagemagick_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_other(): File '%s', saving a volumetric image with an external call to "
                   "ImageMagick only writes the first image slice.",
                   cimg_instance,filename);
#ifdef cimg_use_png
#define _cimg_sie_ext1 "png"
#define _cimg_sie_ext2 "png"
#else
#define _cimg_sie_ext1 "pgm"
#define _cimg_sie_ext2 "ppm"
#endif
      CImg<charT> command(1024), filename_tmp(256);
      std::FILE *file;
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",cimg::temporary_path(),
                      cimg_file_separator,cimg::filenamerand(),_spectrum==1?_cimg_sie_ext1:_cimg_sie_ext2);
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
#ifdef cimg_use_png
      save_png(filename_tmp);
#else
      save_pnm(filename_tmp);
#endif
      cimg_snprintf(command,command._width,"%s -quality %u \"%s\" \"%s\"",
                    cimg::imagemagick_path(),quality,
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      cimg::system(command);
      file = std_fopen(filename,"rb");
      if (!file)
        throw CImgIOException(_cimg_instance
                              "save_imagemagick_external(): Failed to save file '%s' with "
                              "external command 'magick/convert'.",
                              cimg_instance,
                              filename);

      if (file) cimg::fclose(file);
      std::remove(filename_tmp);
      return *this;