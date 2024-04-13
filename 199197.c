    **/
    const CImg<T>& save_gzip_external(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_gzip_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      CImg<charT> command(1024), filename_tmp(256), body(256);
      const char
        *ext = cimg::split_filename(filename,body),
        *ext2 = cimg::split_filename(body,0);
      std::FILE *file;
      do {
        if (!cimg::strcasecmp(ext,"gz")) {
          if (*ext2) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                   cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext2);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.cimg",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        } else {
          if (*ext) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                  cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.cimg",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        }
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
      save(filename_tmp);
      cimg_snprintf(command,command._width,"%s -c \"%s\" > \"%s\"",
                    cimg::gzip_path(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      cimg::system(command);
      file = std_fopen(filename,"rb");
      if (!file)
        throw CImgIOException(_cimg_instance
                              "save_gzip_external(): Failed to save file '%s' with external command 'gzip'.",
                              cimg_instance,
                              filename);

      else cimg::fclose(file);
      std::remove(filename_tmp);
      return *this;