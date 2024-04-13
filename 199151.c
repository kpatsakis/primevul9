     \param mode Output type, can be primary { 0=files only | 1=folders only | 2=files + folders }.
     \param include_path Tell if \c path must be included in resulting filenames.
     \return A list of filenames.
  **/
  inline CImgList<char> files(const char *const path, const bool is_pattern=false,
                              const unsigned int mode=2, const bool include_path=false) {
    if (!path || !*path) return files("*",true,mode,include_path);
    CImgList<char> res;

    // If path is a valid folder name, ignore argument 'is_pattern'.
    const bool _is_pattern = is_pattern && !cimg::is_directory(path);
    bool is_root = false, is_current = false;
    cimg::unused(is_root,is_current);

    // Clean format of input path.
    CImg<char> pattern, _path = CImg<char>::string(path);
#if cimg_OS==2
    for (char *ps = _path; *ps; ++ps) if (*ps=='\\') *ps='/';
#endif
    char *pd = _path;
    for (char *ps = pd; *ps; ++ps) { if (*ps!='/' || *ps!=*(ps+1)) *(pd++) = *ps; }
    *pd = 0;
    unsigned int lp = (unsigned int)std::strlen(_path);
    if (!_is_pattern && lp && _path[lp - 1]=='/') {
      _path[lp - 1] = 0; --lp;
#if cimg_OS!=2
      is_root = !*_path;
#endif
    }

    // Separate folder path and matching pattern.
    if (_is_pattern) {
      const unsigned int bpos = (unsigned int)(cimg::basename(_path,'/') - _path.data());
      CImg<char>::string(_path).move_to(pattern);
      if (bpos) {
        _path[bpos - 1] = 0; // End 'path' at last slash.
#if cimg_OS!=2
        is_root = !*_path;
#endif
      } else { // No path to folder specified, assuming current folder.
        is_current = true; *_path = 0;
      }
      lp = (unsigned int)std::strlen(_path);
    }

    // Windows version.
#if cimg_OS==2
    if (!_is_pattern) {
      pattern.assign(lp + 3);
      std::memcpy(pattern,_path,lp);
      pattern[lp] = '/'; pattern[lp + 1] = '*'; pattern[lp + 2] = 0;
    }
    WIN32_FIND_DATAA file_data;
    const HANDLE dir = FindFirstFileA(pattern.data(),&file_data);
    if (dir==INVALID_HANDLE_VALUE) return CImgList<char>::const_empty();
    do {
      const char *const filename = file_data.cFileName;
      if (*filename!='.' || (filename[1] && (filename[1]!='.' || filename[2]))) {
        const unsigned int lf = (unsigned int)std::strlen(filename);
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0;
        if ((!mode && !is_directory) || (mode==1 && is_directory) || mode>=2) {
          if (include_path) {
            CImg<char> full_filename((lp?lp+1:0) + lf + 1);
            if (lp) { std::memcpy(full_filename,_path,lp); full_filename[lp] = '/'; }
            std::memcpy(full_filename._data + (lp?lp + 1:0),filename,lf + 1);
            full_filename.move_to(res);
          } else CImg<char>(filename,lf + 1).move_to(res);
        }
      }
    } while (FindNextFileA(dir,&file_data));
    FindClose(dir);

    // Unix version (posix).
#elif cimg_OS == 1
    DIR *const dir = opendir(is_root?"/":is_current?".":_path.data());
    if (!dir) return CImgList<char>::const_empty();
    struct dirent *ent;
    while ((ent=readdir(dir))!=0) {
      const char *const filename = ent->d_name;
      if (*filename!='.' || (filename[1] && (filename[1]!='.' || filename[2]))) {
        const unsigned int lf = (unsigned int)std::strlen(filename);
        CImg<char> full_filename(lp + lf + 2);

        if (!is_current) {
          full_filename.assign(lp + lf + 2);
          if (lp) std::memcpy(full_filename,_path,lp);
          full_filename[lp] = '/';
          std::memcpy(full_filename._data + lp + 1,filename,lf + 1);
        } else full_filename.assign(filename,lf + 1);

        struct stat st;
        if (stat(full_filename,&st)==-1) continue;
        const bool is_directory = (st.st_mode & S_IFDIR)!=0;
        if ((!mode && !is_directory) || (mode==1 && is_directory) || mode==2) {
          if (include_path) {
            if (!_is_pattern || (_is_pattern && !fnmatch(pattern,full_filename,0)))
              full_filename.move_to(res);
          } else {
            if (!_is_pattern || (_is_pattern && !fnmatch(pattern,full_filename,0)))
              CImg<char>(filename,lf + 1).move_to(res);
          }
        }
      }
    }
    closedir(dir);
#endif

    // Sort resulting list by lexicographic order.