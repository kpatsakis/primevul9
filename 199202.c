    //! Load an image from a video file using the external tool 'ffmpeg'.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_ffmpeg_external(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load_ffmpeg_external(): Specified filename is (null).",
                                    cimglist_instance);
      std::fclose(cimg::fopen(filename,"rb"));            // Check if file exists.
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      std::FILE *file = 0;
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001.ppm",filename_tmp._data);
        if ((file=std_fopen(filename_tmp2,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%%6d.ppm",filename_tmp._data);
      cimg_snprintf(command,command._width,"%s -i \"%s\" \"%s\"",
                    cimg::ffmpeg_path(),
                    CImg<charT>::string(filename)._system_strescape().data(),
                    CImg<charT>::string(filename_tmp2)._system_strescape().data());
      cimg::system(command,0);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      assign();
      unsigned int i = 1;
      for (bool stop_flag = false; !stop_flag; ++i) {
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u.ppm",filename_tmp._data,i);
        CImg<T> img;
        try { img.load_pnm(filename_tmp2); }
        catch (CImgException&) { stop_flag = true; }
        if (img) { img.move_to(*this); std::remove(filename_tmp2); }
      }
      cimg::exception_mode(omode);
      if (is_empty())
        throw CImgIOException(_cimglist_instance
                              "load_ffmpeg_external(): Failed to open file '%s' with external command 'ffmpeg'.",