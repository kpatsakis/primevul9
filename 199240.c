     \param user_path Specified path, or \c 0 to get the path currently used.
     \param reinit_path Force path to be recalculated (may take some time).
     \return Path where temporary files can be saved.
  **/
  inline const char* temporary_path(const char *const user_path, const bool reinit_path) {
#define _cimg_test_temporary_path(p)                                    \
    if (!path_found) {                                                  \
      cimg_snprintf(s_path,s_path.width(),"%s",p);                      \
      cimg_snprintf(tmp,tmp._width,"%s%c%s",s_path.data(),cimg_file_separator,filename_tmp._data); \
      if ((file=std_fopen(tmp,"wb"))!=0) { cimg::fclose(file); std::remove(tmp); path_found = true; } \
    }
    static CImg<char> s_path;
    cimg::mutex(7);
    if (reinit_path) s_path.assign();
    if (user_path) {
      if (!s_path) s_path.assign(1024);
      std::strncpy(s_path,user_path,1023);
    } else if (!s_path) {
      s_path.assign(1024);
      bool path_found = false;
      CImg<char> tmp(1024), filename_tmp(256);
      std::FILE *file = 0;
      cimg_snprintf(filename_tmp,filename_tmp._width,"%s.tmp",cimg::filenamerand());
      char *tmpPath = std::getenv("TMP");
      if (!tmpPath) { tmpPath = std::getenv("TEMP"); winformat_string(tmpPath); }
      if (tmpPath) _cimg_test_temporary_path(tmpPath);
#if cimg_OS==2
      _cimg_test_temporary_path("C:\\WINNT\\Temp");
      _cimg_test_temporary_path("C:\\WINDOWS\\Temp");
      _cimg_test_temporary_path("C:\\Temp");
      _cimg_test_temporary_path("C:");
      _cimg_test_temporary_path("D:\\WINNT\\Temp");
      _cimg_test_temporary_path("D:\\WINDOWS\\Temp");
      _cimg_test_temporary_path("D:\\Temp");
      _cimg_test_temporary_path("D:");
#else
      _cimg_test_temporary_path("/tmp");
      _cimg_test_temporary_path("/var/tmp");
#endif
      if (!path_found) {
        *s_path = 0;
        std::strncpy(tmp,filename_tmp,tmp._width - 1);
        if ((file=std_fopen(tmp,"wb"))!=0) { cimg::fclose(file); std::remove(tmp); path_found = true; }
      }
      if (!path_found) {
        cimg::mutex(7,0);
        throw CImgIOException("cimg::temporary_path(): Failed to locate path for writing temporary files.\n");
      }