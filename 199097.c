     \param user_path Specified path, or \c 0 to get the path currently used.
     \param reinit_path Force path to be recalculated (may take some time).
     \return Path containing the \c gzip binary.
  **/
  inline const char *gzip_path(const char *const user_path, const bool reinit_path) {
    static CImg<char> s_path;
    cimg::mutex(7);
    if (reinit_path) s_path.assign();
    if (user_path) {
      if (!s_path) s_path.assign(1024);
      std::strncpy(s_path,user_path,1023);
    } else if (!s_path) {
      s_path.assign(1024);
      bool path_found = false;
      std::FILE *file = 0;
#if cimg_OS==2
      if (!path_found) {
        std::strcpy(s_path,".\\gzip.exe");
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      if (!path_found) std::strcpy(s_path,"gzip.exe");
#else
      if (!path_found) {
        std::strcpy(s_path,"./gzip");
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      if (!path_found) std::strcpy(s_path,"gzip");
#endif
      winformat_string(s_path);