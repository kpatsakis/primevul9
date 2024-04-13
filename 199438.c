                              cimglist_instance,filename);
      return *this;
    }

    CImgList<T>& _load_gif_external(const char *const filename, const bool use_graphicsmagick=false) {
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      std::FILE *file = 0;
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        if (use_graphicsmagick) cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png.0",filename_tmp._data);
        else cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s-0.png",filename_tmp._data);
        if ((file=std_fopen(filename_tmp2,"rb"))!=0) cimg::fclose(file);
      } while (file);
      if (use_graphicsmagick) cimg_snprintf(command,command._width,"%s convert \"%s\" \"%s.png\"",
                                            cimg::graphicsmagick_path(),
                                            CImg<charT>::string(filename)._system_strescape().data(),
                                            CImg<charT>::string(filename_tmp)._system_strescape().data());
      else cimg_snprintf(command,command._width,"%s \"%s\" \"%s.png\"",
                         cimg::imagemagick_path(),
                         CImg<charT>::string(filename)._system_strescape().data(),
                         CImg<charT>::string(filename_tmp)._system_strescape().data());
      cimg::system(command,0);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      assign();

      // Try to read a single frame gif.
      cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png",filename_tmp._data);
      CImg<T> img;
      try { img.load_png(filename_tmp2); }
      catch (CImgException&) { }
      if (img) { img.move_to(*this); std::remove(filename_tmp2); }
      else { // Try to read animated gif.
        unsigned int i = 0;
        for (bool stop_flag = false; !stop_flag; ++i) {
          if (use_graphicsmagick) cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s.png.%u",filename_tmp._data,i);
          else cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s-%u.png",filename_tmp._data,i);
          CImg<T> img;
          try { img.load_png(filename_tmp2); }
          catch (CImgException&) { stop_flag = true; }
          if (img) { img.move_to(*this); std::remove(filename_tmp2); }
        }