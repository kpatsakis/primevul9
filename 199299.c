       \param filename Filename to write data to.
       \param fps Number of desired frames per second.
       \param nb_loops Number of loops (\c 0 for infinite looping).
    **/
    const CImgList<T>& save_gif_external(const char *const filename, const float fps=25,
                                         const unsigned int nb_loops=0) {
      CImg<charT> command(1024), filename_tmp(256), filename_tmp2(256);
      CImgList<charT> filenames;
      std::FILE *file = 0;

#ifdef cimg_use_png
#define _cimg_save_gif_ext "png"
#else
#define _cimg_save_gif_ext "ppm"
#endif

      do {
        cimg_snprintf(filename_tmp,filename_tmp._width,"%s%c%s",
                      cimg::temporary_path(),cimg_file_separator,cimg::filenamerand());
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_000001." _cimg_save_gif_ext,filename_tmp._data);
        if ((file=std_fopen(filename_tmp2,"rb"))!=0) cimg::fclose(file);
      } while (file);
      cimglist_for(*this,l) {
        cimg_snprintf(filename_tmp2,filename_tmp2._width,"%s_%.6u." _cimg_save_gif_ext,filename_tmp._data,l + 1);
        CImg<charT>::string(filename_tmp2).move_to(filenames);
        if (_data[l]._depth>1 || _data[l]._spectrum!=3) _data[l].get_resize(-100,-100,1,3).save(filename_tmp2);
        else _data[l].save(filename_tmp2);
      }
      cimg_snprintf(command,command._width,"%s -delay %u -loop %u",
                    cimg::imagemagick_path(),(unsigned int)std::max(0.0f,cimg::round(100/fps)),nb_loops);
      CImg<ucharT>::string(command).move_to(filenames,0);
      cimg_snprintf(command,command._width,"\"%s\"",
                    CImg<charT>::string(filename)._system_strescape().data());
      CImg<ucharT>::string(command).move_to(filenames);
      CImg<charT> _command = filenames>'x';
      cimg_for(_command,p,char) if (!*p) *p = ' ';
      _command.back() = 0;

      cimg::system(_command);
      file = std_fopen(filename,"rb");
      if (!file)
        throw CImgIOException(_cimglist_instance
                              "save_gif_external(): Failed to save file '%s' with external command 'magick/convert'.",
                              cimglist_instance,
                              filename);