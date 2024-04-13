    **/
    const CImg<T>& save(const char *const filename, const int number=-1, const unsigned int digits=6) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save(): Specified filename is (null).",
                                    cimg_instance);
      // Do not test for empty instances, since .cimg format is able to manage empty instances.
      const bool is_stdout = *filename=='-' && (!filename[1] || filename[1]=='.');
      const char *const ext = cimg::split_filename(filename);
      CImg<charT> nfilename(1024);
      const char *const fn = is_stdout?filename:(number>=0)?cimg::number_filename(filename,number,digits,nfilename):
        filename;

#ifdef cimg_save_plugin
      cimg_save_plugin(fn);
#endif
#ifdef cimg_save_plugin1
      cimg_save_plugin1(fn);
#endif
#ifdef cimg_save_plugin2
      cimg_save_plugin2(fn);
#endif
#ifdef cimg_save_plugin3
      cimg_save_plugin3(fn);
#endif
#ifdef cimg_save_plugin4
      cimg_save_plugin4(fn);
#endif
#ifdef cimg_save_plugin5
      cimg_save_plugin5(fn);
#endif
#ifdef cimg_save_plugin6
      cimg_save_plugin6(fn);
#endif
#ifdef cimg_save_plugin7
      cimg_save_plugin7(fn);
#endif
#ifdef cimg_save_plugin8
      cimg_save_plugin8(fn);
#endif
      // Ascii formats
      if (!cimg::strcasecmp(ext,"asc")) return save_ascii(fn);
      else if (!cimg::strcasecmp(ext,"dlm") ||
               !cimg::strcasecmp(ext,"txt")) return save_dlm(fn);
      else if (!cimg::strcasecmp(ext,"cpp") ||
               !cimg::strcasecmp(ext,"hpp") ||
               !cimg::strcasecmp(ext,"h") ||
               !cimg::strcasecmp(ext,"c")) return save_cpp(fn);

      // 2d binary formats
      else if (!cimg::strcasecmp(ext,"bmp")) return save_bmp(fn);
      else if (!cimg::strcasecmp(ext,"jpg") ||
               !cimg::strcasecmp(ext,"jpeg") ||
               !cimg::strcasecmp(ext,"jpe") ||
               !cimg::strcasecmp(ext,"jfif") ||
               !cimg::strcasecmp(ext,"jif")) return save_jpeg(fn);
      else if (!cimg::strcasecmp(ext,"rgb")) return save_rgb(fn);
      else if (!cimg::strcasecmp(ext,"rgba")) return save_rgba(fn);
      else if (!cimg::strcasecmp(ext,"png")) return save_png(fn);
      else if (!cimg::strcasecmp(ext,"pgm") ||
               !cimg::strcasecmp(ext,"ppm") ||
               !cimg::strcasecmp(ext,"pnm")) return save_pnm(fn);
      else if (!cimg::strcasecmp(ext,"pnk")) return save_pnk(fn);
      else if (!cimg::strcasecmp(ext,"pfm")) return save_pfm(fn);
      else if (!cimg::strcasecmp(ext,"exr")) return save_exr(fn);
      else if (!cimg::strcasecmp(ext,"tif") ||
               !cimg::strcasecmp(ext,"tiff")) return save_tiff(fn);

      // 3d binary formats
      else if (!cimg::strcasecmp(ext,"cimgz")) return save_cimg(fn,true);
      else if (!cimg::strcasecmp(ext,"cimg") || !*ext) return save_cimg(fn,false);
      else if (!cimg::strcasecmp(ext,"dcm")) return save_medcon_external(fn);
      else if (!cimg::strcasecmp(ext,"hdr") ||
               !cimg::strcasecmp(ext,"nii")) return save_analyze(fn);
      else if (!cimg::strcasecmp(ext,"inr")) return save_inr(fn);
      else if (!cimg::strcasecmp(ext,"mnc")) return save_minc2(fn);
      else if (!cimg::strcasecmp(ext,"pan")) return save_pandore(fn);
      else if (!cimg::strcasecmp(ext,"raw")) return save_raw(fn);

      // Archive files
      else if (!cimg::strcasecmp(ext,"gz")) return save_gzip_external(fn);

      // Image sequences
      else if (!cimg::strcasecmp(ext,"yuv")) return save_yuv(fn,444,true);
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
               !cimg::strcasecmp(ext,"mpeg")) return save_video(fn);
      return save_other(fn);