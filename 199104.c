     \param user_path Specified path, or \c 0 to get the path currently used.
     \param reinit_path Force path to be recalculated (may take some time).
     \return Path containing the \c convert binary.
  **/
  inline const char* imagemagick_path(const char *const user_path, const bool reinit_path) {
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
      for (int l = 0; l<2 && !path_found; ++l) {
        const char *const s_exe = l?"convert":"magick";
        cimg_snprintf(s_path,s_path._width,".\\%s.exe",s_exe);
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%.2d-\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d-Q\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"%s\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",pf_path,k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%.2d-\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d-Q\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"C:\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%.2d-\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d-Q\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=10 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%.2d-\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 9; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d-Q\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        for (int k = 32; k>=0 && !path_found; --k) {
          cimg_snprintf(s_path,s_path._width,"D:\\IMAGEM~1.%d\\VISUA~1\\BIN\\%s.exe",k,s_exe);
          if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
        }
        if (!path_found) cimg_snprintf(s_path,s_path._width,"%s.exe",s_exe);
      }
#else
      std::strcpy(s_path,"./magick");
      if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      if (!path_found) {
        std::strcpy(s_path,"./convert");
        if ((file=std_fopen(s_path,"r"))!=0) { cimg::fclose(file); path_found = true; }
      }
      if (!path_found) std::strcpy(s_path,"convert");
#endif
      winformat_string(s_path);