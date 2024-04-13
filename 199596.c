     \param referer Referer used, as a C-string.
     \return Value of \c filename_local.
     \note Use the \c libcurl library, or the external binaries \c wget or \c curl to perform the download.
  **/
  inline char *load_network(const char *const url, char *const filename_local,
                            const unsigned int timeout, const bool try_fallback,
                            const char *const referer) {
    if (!url)
      throw CImgArgumentException("cimg::load_network(): Specified URL is (null).");
    if (!filename_local)
      throw CImgArgumentException("cimg::load_network(): Specified destination string is (null).");

    const char *const __ext = cimg::split_filename(url), *const _ext = (*__ext && __ext>url)?__ext - 1:__ext;
    CImg<char> ext = CImg<char>::string(_ext);
    std::FILE *file = 0;
    *filename_local = 0;
    if (ext._width>16 || !cimg::strncasecmp(ext,"cgi",3)) *ext = 0;
    else cimg::strwindows_reserved(ext);
    do {
      cimg_snprintf(filename_local,256,"%s%c%s%s",
                    cimg::temporary_path(),cimg_file_separator,cimg::filenamerand(),ext._data);
      if ((file=std_fopen(filename_local,"rb"))!=0) cimg::fclose(file);
    } while (file);

#ifdef cimg_use_curl
    const unsigned int omode = cimg::exception_mode();
    cimg::exception_mode(0);
    try {
      CURL *curl = 0;
      CURLcode res;
      curl = curl_easy_init();
      if (curl) {
        file = cimg::fopen(filename_local,"wb");
        curl_easy_setopt(curl,CURLOPT_URL,url);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,0);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,file);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        if (timeout) curl_easy_setopt(curl,CURLOPT_TIMEOUT,(long)timeout);
        if (std::strchr(url,'?')) curl_easy_setopt(curl,CURLOPT_HTTPGET,1L);
        if (referer) curl_easy_setopt(curl,CURLOPT_REFERER,referer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        cimg::fseek(file,0,SEEK_END); // Check if file size is 0.
        const cimg_ulong siz = cimg::ftell(file);
        cimg::fclose(file);
        if (siz>0 && res==CURLE_OK) {
          cimg::exception_mode(omode);
          return filename_local;
        } else std::remove(filename_local);
      }
    } catch (...) { }
    cimg::exception_mode(omode);
    if (!try_fallback) throw CImgIOException("cimg::load_network(): Failed to load file '%s' with libcurl.",url);
#endif

    CImg<char> command((unsigned int)std::strlen(url) + 64);
    cimg::unused(try_fallback);

    // Try with 'curl' first.
    if (timeout) {
      if (referer)
        cimg_snprintf(command,command._width,"%s -e %s -m %u -f --silent --compressed -o \"%s\" \"%s\"",
                      cimg::curl_path(),referer,timeout,filename_local,url);
      else
        cimg_snprintf(command,command._width,"%s -m %u -f --silent --compressed -o \"%s\" \"%s\"",
                      cimg::curl_path(),timeout,filename_local,url);
    } else {
      if (referer)
        cimg_snprintf(command,command._width,"%s -e %s -f --silent --compressed -o \"%s\" \"%s\"",
                      cimg::curl_path(),referer,filename_local,url);
      else
        cimg_snprintf(command,command._width,"%s -f --silent --compressed -o \"%s\" \"%s\"",
                      cimg::curl_path(),filename_local,url);
    }
    cimg::system(command);

    if (!(file = std_fopen(filename_local,"rb"))) {

      // Try with 'wget' otherwise.
      if (timeout) {
        if (referer)
          cimg_snprintf(command,command._width,"%s --referer=%s -T %u -q -r -l 0 --no-cache -O \"%s\" \"%s\"",
                        cimg::wget_path(),referer,timeout,filename_local,url);
        else
          cimg_snprintf(command,command._width,"%s -T %u -q -r -l 0 --no-cache -O \"%s\" \"%s\"",
                        cimg::wget_path(),timeout,filename_local,url);
      } else {
        if (referer)
          cimg_snprintf(command,command._width,"%s --referer=%s -q -r -l 0 --no-cache -O \"%s\" \"%s\"",
                        cimg::wget_path(),referer,filename_local,url);
        else
          cimg_snprintf(command,command._width,"%s -q -r -l 0 --no-cache -O \"%s\" \"%s\"",
                        cimg::wget_path(),filename_local,url);
      }
      cimg::system(command);

      if (!(file = std_fopen(filename_local,"rb")))
        throw CImgIOException("cimg::load_network(): Failed to load file '%s' with external commands "
                              "'wget' or 'curl'.",url);
      cimg::fclose(file);

      // Try gunzip it.
      cimg_snprintf(command,command._width,"%s.gz",filename_local);
      std::rename(filename_local,command);
      cimg_snprintf(command,command._width,"%s --quiet \"%s.gz\"",
                    gunzip_path(),filename_local);
      cimg::system(command);
      file = std_fopen(filename_local,"rb");
      if (!file) {
        cimg_snprintf(command,command._width,"%s.gz",filename_local);
        std::rename(command,filename_local);
        file = std_fopen(filename_local,"rb");
      }
    }
    cimg::fseek(file,0,SEEK_END); // Check if file size is 0.
    if (std::ftell(file)<=0)
      throw CImgIOException("cimg::load_network(): Failed to load URL '%s' with external commands "