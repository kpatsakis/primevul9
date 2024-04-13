    **/
    inline int system(const char *const command, const char *const module_name=0) {
      cimg::unused(module_name);
#ifdef cimg_no_system_calls
      return -1;
#else
#if cimg_OS==1
      const unsigned int l = (unsigned int)std::strlen(command);
      if (l) {
        char *const ncommand = new char[l + 24];
        std::strncpy(ncommand,command,l);
        std::strcpy(ncommand + l," >/dev/null 2>&1"); // Make command silent.
        const int out_val = std::system(ncommand);
        delete[] ncommand;
        return out_val;
      } else return -1;
#elif cimg_OS==2
      PROCESS_INFORMATION pi;
      STARTUPINFO si;
      std::memset(&pi,0,sizeof(PROCESS_INFORMATION));
      std::memset(&si,0,sizeof(STARTUPINFO));
      GetStartupInfo(&si);
      si.cb = sizeof(si);
      si.wShowWindow = SW_HIDE;
      si.dwFlags |= SW_HIDE | STARTF_USESHOWWINDOW;
      const BOOL res = CreateProcess((LPCTSTR)module_name,(LPTSTR)command,0,0,FALSE,0,0,0,&si,&pi);
      if (res) {
        WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return 0;
      } else return std::system(command);
#else
      return std::system(command);
#endif
#endif