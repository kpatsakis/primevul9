    **/
    CImg<T>& load_medcon_external(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_medcon_external(): Specified filename is (null).",
                                    cimg_instance);
      std::fclose(cimg::fopen(filename,"rb"));            // Check if file exists.
      CImg<charT> command(1024), filename_tmp(256), body(256);
      cimg::fclose(cimg::fopen(filename,"r"));
      std::FILE *file = 0;
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s.hdr",cimg::filenamerand());
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimg_snprintf(command,command._width,"%s -w -c anlz -o \"%s\" -f \"%s\"",
                    cimg::medcon_path(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    CImg<charT>::string(filename)._system_strescape().data());
      cimg::system(command);
      cimg::split_filename(filename_tmp,body);

      cimg_snprintf(command,command._width,"%s.hdr",body._data);
      file = std_fopen(command,"rb");
      if (!file) {
        cimg_snprintf(command,command._width,"m000-%s.hdr",body._data);
        file = std_fopen(command,"rb");
        if (!file) {
          throw CImgIOException(_cimg_instance
                                "load_medcon_external(): Failed to load file '%s' with external command 'medcon'.",
                                cimg_instance,
                                filename);
        }
      }
      cimg::fclose(file);
      load_analyze(command);
      std::remove(command);
      cimg::split_filename(command,body);
      cimg_snprintf(command,command._width,"%s.img",body._data);
      std::remove(command);
      return *this;