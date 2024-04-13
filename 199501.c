     \param reinit_path Force path to be recalculated (may take some time).
     \return Path containing the program files.
  **/
#if cimg_OS==2
  inline const char* programfiles_path(const char *const user_path, const bool reinit_path) {
    static CImg<char> s_path;
    cimg::mutex(7);
    if (reinit_path) s_path.assign();
    if (user_path) {
      if (!s_path) s_path.assign(1024);
      std::strncpy(s_path,user_path,1023);
    } else if (!s_path) {
      s_path.assign(MAX_PATH);
      *s_path = 0;
      // Note: in the following line, 0x26 = CSIDL_PROGRAM_FILES (not defined on every compiler).
#if !defined(__INTEL_COMPILER)
      if (!SHGetSpecialFolderPathA(0,s_path,0x0026,false)) {
        const char *const pfPath = std::getenv("PROGRAMFILES");
        if (pfPath) std::strncpy(s_path,pfPath,MAX_PATH - 1);
        else std::strcpy(s_path,"C:\\PROGRA~1");
      }
#else
      std::strcpy(s_path,"C:\\PROGRA~1");
#endif