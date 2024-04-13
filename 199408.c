    **/
    CImg<T>& load_gzip_external(const char *const filename) {
      if (!filename)
        throw CImgIOException(_cimg_instance
                              "load_gzip_external(): Specified filename is (null).",
                              cimg_instance);
      std::fclose(cimg::fopen(filename,"rb"));            // Check if file exists.
      CImg<charT> command(1024), filename_tmp(256), body(256);
      const char
        *const ext = cimg::split_filename(filename,body),
        *const ext2 = cimg::split_filename(body,0);

      std::FILE *file = 0;
      do {
        if (!cimg::strcasecmp(ext,"gz")) {
          if (*ext2) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                   cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext2);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        } else {
          if (*ext) cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.%s",
                                  cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext);
          else cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                             cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        }
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimg_snprintf(command,command._width,"%s -c \"%s\" > \"%s\"",
                    cimg::gunzip_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command);
      if (!(file = std_fopen(filename_tmp,"rb"))) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_gzip_external(): Failed to load file '%s' with external command 'gunzip'.",
                              cimg_instance,
                              filename);

      } else cimg::fclose(file);
      load(filename_tmp);
      std::remove(filename_tmp);
      return *this;