    //! Load gif file, using ImageMagick or GraphicsMagick's external tools.
    /**
      \param filename Filename to read data from.
    **/
    CImgList<T>& load_gif_external(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load_gif_external(): Specified filename is (null).",
                                    cimglist_instance);
      std::fclose(cimg::fopen(filename,"rb"));            // Check if file exists.
      if (!_load_gif_external(filename,false))
        if (!_load_gif_external(filename,true))
          try { assign(CImg<T>().load_other(filename)); } catch (CImgException&) { assign(); }
      if (is_empty())
        throw CImgIOException(_cimglist_instance