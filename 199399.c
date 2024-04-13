     \param user_path Specified path, or \c 0 to get the path currently used.
     \param reinit_path Force path to be recalculated (may take some time).
     \return Path containing the \c gm binary.
  **/
  inline const char* graphicsmagick_path(const char *const user_path, const bool reinit_path) {
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
      const char *const pf_path = programfiles_path();
      if (!path_found) {
        std::strcpy(s_path,".\\gm.exe");
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%.2d-\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d-Q\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"%s\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",pf_path,k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%.2d-\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d-Q\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"C:\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%.2d-\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d-Q\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=10 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%.2d-\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 9; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d-Q\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      for (int k = 32; k>=0 && !path_found; --k) {
        cimg_snprintf(s_path,s_path._width,"D:\\GRAPHI~1.%d\\VISUA~1\\BIN\\gm.exe",k);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      if (!path_found) std::strcpy(s_path,"gm.exe");
#else
      if (!path_found) {
        std::strcpy(s_path,"./gm");
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      if (!path_found) std::strcpy(s_path,"gm");
#endif
      winformat_string(s_path);