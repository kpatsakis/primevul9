    **/
    CImg<T>& load_other(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_other(): Specified filename is (null).",
                                    cimg_instance);

      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try { load_magick(filename); }
      catch (CImgException&) {
        try { load_imagemagick_external(filename); }
        catch (CImgException&) {
          try { load_graphicsmagick_external(filename); }
          catch (CImgException&) {
            try { load_cimg(filename); }
            catch (CImgException&) {
              try {
                std::fclose(cimg::fopen(filename,"rb"));
              } catch (CImgException&) {
                cimg::exception_mode(omode);
                throw CImgIOException(_cimg_instance
                                      "load_other(): Failed to open file '%s'.",
                                      cimg_instance,
                                      filename);
              }
              cimg::exception_mode(omode);
              throw CImgIOException(_cimg_instance
                                    "load_other(): Failed to recognize format of file '%s'.",
                                    cimg_instance,
                                    filename);
            }
          }
        }
      }
      cimg::exception_mode(omode);
      return *this;