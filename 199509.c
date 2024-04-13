    **/
    const CImg<T>& save_medcon_external(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_medcon_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      CImg<charT> command(1024), filename_tmp(256), body(256);
      std::FILE *file;
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s.hdr",cimg::filenamerand());
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
      save_analyze(filename_tmp);
      cimg_snprintf(command,command._width,"%s -w -c dicom -o \"%s\" -f \"%s\"",
                    cimg::medcon_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command);
      std::remove(filename_tmp);
      cimg::split_filename(filename_tmp,body);
      cimg_snprintf(filename_tmp,filename_tmp._width,"%s.img",body._data);
      std::remove(filename_tmp);

      file = std_fopen(filename,"rb");
      if (!file) {
        cimg_snprintf(command,command._width,"m000-%s",filename);
        file = std_fopen(command,"rb");
        if (!file) {
          cimg::fclose(cimg::fopen(filename,"r"));
          throw CImgIOException(_cimg_instance
                                "save_medcon_external(): Failed to save file '%s' with external command 'medcon'.",
                                cimg_instance,
                                filename);
        }
      }
      cimg::fclose(file);
      std::rename(command,filename);
      return *this;