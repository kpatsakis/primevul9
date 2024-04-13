#endif
  }

  // Open a file (with wide character support on Windows).
  inline std::FILE *win_fopen(const char *const path, const char *const mode) {
#if cimg_OS==2
    // Convert 'path' to a wide-character string.
    int err = MultiByteToWideChar(CP_UTF8,0,path,-1,0,0);
    if (!err) return std_fopen(path,mode);
    CImg<wchar_t> wpath(err);
    err = MultiByteToWideChar(CP_UTF8,0,path,-1,wpath,err);
    if (!err) return std_fopen(path,mode);

    // Convert 'mode' to a wide-character string.
    err = MultiByteToWideChar(CP_UTF8,0,mode,-1,0,0);
    if (!err) return std_fopen(path,mode);
    CImg<wchar_t> wmode(err);
    err = MultiByteToWideChar(CP_UTF8,0,mode,-1,wmode,err);
    if (!err) return std_fopen(path,mode);
    return _wfopen(wpath,wmode);