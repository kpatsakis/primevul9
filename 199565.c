    **/
    inline std::FILE *fopen(const char *const path, const char *const mode) {
      if (!path)
        throw CImgArgumentException("cimg::fopen(): Specified file path is (null).");
      if (!mode)
        throw CImgArgumentException("cimg::fopen(): File '%s', specified mode is (null).",
                                    path);
      std::FILE *res = 0;
      if (*path=='-' && (!path[1] || path[1]=='.')) {
        res = (*mode=='r')?cimg::_stdin():cimg::_stdout();
#if cimg_OS==2
        if (*mode && mode[1]=='b') { // Force stdin/stdout to be in binary mode.
#ifdef __BORLANDC__
          if (setmode(_fileno(res),0x8000)==-1) res = 0;
#else
          if (_setmode(_fileno(res),0x8000)==-1) res = 0;
#endif
        }
#endif
      } else res = std_fopen(path,mode);
      if (!res) throw CImgIOException("cimg::fopen(): Failed to open file '%s' with mode '%s'.",
                                      path,mode);
      return res;