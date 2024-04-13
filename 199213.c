    **/
    CImg<T>& load_graphicsmagick_external(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_graphicsmagick_external(): Specified filename is (null).",
                                    cimg_instance);
      std::fclose(cimg::fopen(filename,"rb"));            // Check if file exists.
      CImg<charT> command(1024), filename_tmp(256);
      std::FILE *file = 0;
      const CImg<charT> s_filename = CImg<charT>::string(filename)._system_strescape();
#if cimg_OS==1
      if (!cimg::system("which gm")) {
        cimg_snprintf(command,command._width,"%s convert \"%s\" pnm:-",
                      cimg::graphicsmagick_path(),s_filename.data());
        file = popen(command,"r");
        if (file) {
          const unsigned int omode = cimg::exception_mode();
          cimg::exception_mode(0);
          try { load_pnm(file); } catch (...) {
            pclose(file);
            cimg::exception_mode(omode);
            throw CImgIOException(_cimg_instance
                                  "load_graphicsmagick_external(): Failed to load file '%s' "
                                  "with external command 'gm'.",
                                  cimg_instance,
                                  filename);
          }
          pclose(file);
          return *this;
        }
      }
#endif
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s.pnm",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        if ((file=std_fopen(filename_tmp,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimg_snprintf(command,command._width,"%s convert \"%s\" \"%s\"",
                    cimg::graphicsmagick_path(),s_filename.data(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command,cimg::graphicsmagick_path());
      if (!(file = std_fopen(filename_tmp,"rb"))) {
        cimg::fclose(cimg::fopen(filename,"r"));
        throw CImgIOException(_cimg_instance
                              "load_graphicsmagick_external(): Failed to load file '%s' with external command 'gm'.",
                              cimg_instance,
                              filename);

      } else cimg::fclose(file);
      load_pnm(filename_tmp);
      std::remove(filename_tmp);
      return *this;