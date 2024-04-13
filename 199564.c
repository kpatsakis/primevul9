    //! Load a list from a file.
    /**
     \param filename Filename to read data from.
    **/
    CImgList<T>& load(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "load(): Specified filename is (null).",
                                    cimglist_instance);

      if (!cimg::strncasecmp(filename,"http://",7) || !cimg::strncasecmp(filename,"https://",8)) {
        CImg<charT> filename_local(256);
        load(cimg::load_network(filename,filename_local));
        std::remove(filename_local);
        return *this;
      }

      const bool is_stdin = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      bool is_loaded = true;
      try {
#ifdef cimglist_load_plugin
        cimglist_load_plugin(filename);
#endif
#ifdef cimglist_load_plugin1
        cimglist_load_plugin1(filename);
#endif
#ifdef cimglist_load_plugin2
        cimglist_load_plugin2(filename);
#endif
#ifdef cimglist_load_plugin3
        cimglist_load_plugin3(filename);
#endif
#ifdef cimglist_load_plugin4
        cimglist_load_plugin4(filename);
#endif
#ifdef cimglist_load_plugin5
        cimglist_load_plugin5(filename);
#endif
#ifdef cimglist_load_plugin6
        cimglist_load_plugin6(filename);
#endif
#ifdef cimglist_load_plugin7
        cimglist_load_plugin7(filename);
#endif
#ifdef cimglist_load_plugin8
        cimglist_load_plugin8(filename);
#endif
        if (!cimg::strcasecmp(ext,"tif") ||
            !cimg::strcasecmp(ext,"tiff")) load_tiff(filename);
        else if (!cimg::strcasecmp(ext,"gif")) load_gif_external(filename);
        else if (!cimg::strcasecmp(ext,"cimg") ||
                 !cimg::strcasecmp(ext,"cimgz") ||
                 !*ext) load_cimg(filename);
        else if (!cimg::strcasecmp(ext,"rec") ||
                 !cimg::strcasecmp(ext,"par")) load_parrec(filename);
        else if (!cimg::strcasecmp(ext,"avi") ||
                 !cimg::strcasecmp(ext,"mov") ||
                 !cimg::strcasecmp(ext,"asf") ||
                 !cimg::strcasecmp(ext,"divx") ||
                 !cimg::strcasecmp(ext,"flv") ||
                 !cimg::strcasecmp(ext,"mpg") ||
                 !cimg::strcasecmp(ext,"m1v") ||
                 !cimg::strcasecmp(ext,"m2v") ||
                 !cimg::strcasecmp(ext,"m4v") ||
                 !cimg::strcasecmp(ext,"mjp") ||
                 !cimg::strcasecmp(ext,"mp4") ||
                 !cimg::strcasecmp(ext,"mkv") ||
                 !cimg::strcasecmp(ext,"mpe") ||
                 !cimg::strcasecmp(ext,"movie") ||
                 !cimg::strcasecmp(ext,"ogm") ||
                 !cimg::strcasecmp(ext,"ogg") ||
                 !cimg::strcasecmp(ext,"ogv") ||
                 !cimg::strcasecmp(ext,"qt") ||
                 !cimg::strcasecmp(ext,"rm") ||
                 !cimg::strcasecmp(ext,"vob") ||
                 !cimg::strcasecmp(ext,"wmv") ||
                 !cimg::strcasecmp(ext,"xvid") ||
                 !cimg::strcasecmp(ext,"mpeg")) load_video(filename);
        else if (!cimg::strcasecmp(ext,"gz")) load_gzip_external(filename);
        else is_loaded = false;
      } catch (CImgIOException&) { is_loaded = false; }

      // If nothing loaded, try to guess file format from magic number in file.
      if (!is_loaded && !is_stdin) {
        std::FILE *const file = std_fopen(filename,"rb");
        if (!file) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimglist_instance
                                "load(): Failed to open file '%s'.",
                                cimglist_instance,
                                filename);
        }

        const char *const f_type = cimg::ftype(file,filename);
        std::fclose(file);
        is_loaded = true;
        try {
          if (!cimg::strcasecmp(f_type,"gif")) load_gif_external(filename);
          else if (!cimg::strcasecmp(f_type,"tif")) load_tiff(filename);
          else is_loaded = false;
        } catch (CImgIOException&) { is_loaded = false; }
      }

      // If nothing loaded, try to load file as a single image.
      if (!is_loaded) {
        assign(1);
        try {
          _data->load(filename);
        } catch (CImgIOException&) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimglist_instance
                                "load(): Failed to recognize format of file '%s'.",
                                cimglist_instance,
                                filename);
        }