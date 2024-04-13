    **/
    CImg<T>& load(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load(): Specified filename is (null).",
                                    cimg_instance);

      if (!cimg::strncasecmp(filename,"http://",7) || !cimg::strncasecmp(filename,"https://",8)) {
        CImg<charT> filename_local(256);
        load(cimg::load_network(filename,filename_local));
        std::remove(filename_local);
        return *this;
      }

      const char *const ext = cimg::split_filename(filename);
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      bool is_loaded = true;
      try {
#ifdef cimg_load_plugin
        cimg_load_plugin(filename);
#endif
#ifdef cimg_load_plugin1
        cimg_load_plugin1(filename);
#endif
#ifdef cimg_load_plugin2
        cimg_load_plugin2(filename);
#endif
#ifdef cimg_load_plugin3
        cimg_load_plugin3(filename);
#endif
#ifdef cimg_load_plugin4
        cimg_load_plugin4(filename);
#endif
#ifdef cimg_load_plugin5
        cimg_load_plugin5(filename);
#endif
#ifdef cimg_load_plugin6
        cimg_load_plugin6(filename);
#endif
#ifdef cimg_load_plugin7
        cimg_load_plugin7(filename);
#endif
#ifdef cimg_load_plugin8
        cimg_load_plugin8(filename);
#endif
        // Ascii formats
        if (!cimg::strcasecmp(ext,"asc")) load_ascii(filename);
        else if (!cimg::strcasecmp(ext,"dlm") ||
                 !cimg::strcasecmp(ext,"txt")) load_dlm(filename);

        // 2d binary formats
        else if (!cimg::strcasecmp(ext,"bmp")) load_bmp(filename);
        else if (!cimg::strcasecmp(ext,"jpg") ||
                 !cimg::strcasecmp(ext,"jpeg") ||
                 !cimg::strcasecmp(ext,"jpe") ||
                 !cimg::strcasecmp(ext,"jfif") ||
                 !cimg::strcasecmp(ext,"jif")) load_jpeg(filename);
        else if (!cimg::strcasecmp(ext,"png")) load_png(filename);
        else if (!cimg::strcasecmp(ext,"ppm") ||
                 !cimg::strcasecmp(ext,"pgm") ||
                 !cimg::strcasecmp(ext,"pnm") ||
                 !cimg::strcasecmp(ext,"pbm") ||
                 !cimg::strcasecmp(ext,"pnk")) load_pnm(filename);
        else if (!cimg::strcasecmp(ext,"pfm")) load_pfm(filename);
        else if (!cimg::strcasecmp(ext,"tif") ||
                 !cimg::strcasecmp(ext,"tiff")) load_tiff(filename);
        else if (!cimg::strcasecmp(ext,"exr")) load_exr(filename);
        else if (!cimg::strcasecmp(ext,"cr2") ||
                 !cimg::strcasecmp(ext,"crw") ||
                 !cimg::strcasecmp(ext,"dcr") ||
                 !cimg::strcasecmp(ext,"mrw") ||
                 !cimg::strcasecmp(ext,"nef") ||
                 !cimg::strcasecmp(ext,"orf") ||
                 !cimg::strcasecmp(ext,"pix") ||
                 !cimg::strcasecmp(ext,"ptx") ||
                 !cimg::strcasecmp(ext,"raf") ||
                 !cimg::strcasecmp(ext,"srf")) load_dcraw_external(filename);
        else if (!cimg::strcasecmp(ext,"gif")) load_gif_external(filename);

        // 3d binary formats
        else if (!cimg::strcasecmp(ext,"dcm") ||
                 !cimg::strcasecmp(ext,"dicom")) load_medcon_external(filename);
        else if (!cimg::strcasecmp(ext,"hdr") ||
                 !cimg::strcasecmp(ext,"nii")) load_analyze(filename);
        else if (!cimg::strcasecmp(ext,"par") ||
                 !cimg::strcasecmp(ext,"rec")) load_parrec(filename);
        else if (!cimg::strcasecmp(ext,"mnc")) load_minc2(filename);
        else if (!cimg::strcasecmp(ext,"inr")) load_inr(filename);
        else if (!cimg::strcasecmp(ext,"pan")) load_pandore(filename);
        else if (!cimg::strcasecmp(ext,"cimg") ||
                 !cimg::strcasecmp(ext,"cimgz") ||
                 !*ext)  return load_cimg(filename);

        // Archive files
        else if (!cimg::strcasecmp(ext,"gz")) load_gzip_external(filename);

        // Image sequences
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
        else is_loaded = false;
      } catch (CImgIOException&) { is_loaded = false; }

      // If nothing loaded, try to guess file format from magic number in file.
      if (!is_loaded) {
        std::FILE *file = std_fopen(filename,"rb");
        if (!file) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load(): Failed to open file '%s'.",
                                cimg_instance,
                                filename);
        }

        const char *const f_type = cimg::ftype(file,filename);
        std::fclose(file);
        is_loaded = true;
        try {
          if (!cimg::strcasecmp(f_type,"pnm")) load_pnm(filename);
          else if (!cimg::strcasecmp(f_type,"pfm")) load_pfm(filename);
          else if (!cimg::strcasecmp(f_type,"bmp")) load_bmp(filename);
          else if (!cimg::strcasecmp(f_type,"inr")) load_inr(filename);
          else if (!cimg::strcasecmp(f_type,"jpg")) load_jpeg(filename);
          else if (!cimg::strcasecmp(f_type,"pan")) load_pandore(filename);
          else if (!cimg::strcasecmp(f_type,"png")) load_png(filename);
          else if (!cimg::strcasecmp(f_type,"tif")) load_tiff(filename);
          else if (!cimg::strcasecmp(f_type,"gif")) load_gif_external(filename);
          else if (!cimg::strcasecmp(f_type,"dcm")) load_medcon_external(filename);
          else is_loaded = false;
        } catch (CImgIOException&) { is_loaded = false; }
      }

      // If nothing loaded, try to load file with other means.
      if (!is_loaded) {
        try {
          load_other(filename);
        } catch (CImgIOException&) {
          cimg::exception_mode(omode);
          throw CImgIOException(_cimg_instance
                                "load(): Failed to recognize format of file '%s'.",
                                cimg_instance,
                                filename);
        }
      }
      cimg::exception_mode(omode);
      return *this;