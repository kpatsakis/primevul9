      \param fps Number of frames per second.
      \param codec Type of compression.
      \param bitrate Output bitrate
    **/
    const CImgList<T>& save_ffmpeg_external(const char *const filename, const unsigned int fps=25,
                                            const char *const codec=0, const unsigned int bitrate=2048) const {
      if (!filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_ffmpeg_external(): Specified filename is (null).",
                                    cimglist_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      const char
        *const ext = cimg::split_filename(filename),
        *const _codec = codec?codec:!cimg::strcasecmp(ext,"flv")?"flv":"mpeg2video";

      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      CImgList<charT> filenames;
      std::FILE *file = 0;
      cimglist_for(*this,l) if (!_data[l].is_sameXYZ(_data[0]))
        throw CImgInstanceException(_cimglist_instance
                                    "save_ffmpeg_external(): Invalid instance dimensions for file '%s'.",
                                    cimglist_instance,
                                    filename);
      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001.ppm",filename_tmp._data);
        if ((file=std_fopen(filename_tmp2,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimglist_for(*this,l) {
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u.ppm",filename_tmp._data,l + 1);
        CImg<charT>::string(filename_tmp2).move_to(filenames);
        if (_data[l]._depth>1 || _data[l]._spectrum!=3) _data[l].get_resize(-100,-100,1,3).save_pnm(filename_tmp2);
        else _data[l].save_pnm(filename_tmp2);
      }
      cimg_snprintf(command,command._width,"%s -i \"%s_%%6d.ppm\" -vcodec %s -b %uk -r %u -y \"%s\"",
                    cimg::ffmpeg_path(),
                    CImg<charT>::string(filename_tmp)._system_strescape().data(),
                    _codec,bitrate,fps,
                    CImg<charT>::string(filename)._system_strescape().data());
      cimg::system(command);
      file = std_fopen(filename,"rb");
      if (!file)
        throw CImgIOException(_cimglist_instance
                              "save_ffmpeg_external(): Failed to save file '%s' with external command 'ffmpeg'.",
                              cimglist_instance,
                              filename);