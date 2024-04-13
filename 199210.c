    **/
    inline const char *split_filename(const char *const filename, char *const body=0) {
      if (!filename) { if (body) *body = 0; return 0; }
      const char *p = 0; for (const char *np = filename; np>=filename && (p=np); np = std::strchr(np,'.') + 1) {}
      if (p==filename) {
        if (body) std::strcpy(body,filename);
        return filename + std::strlen(filename);
      }
      const unsigned int l = (unsigned int)(p - filename - 1);
      if (body) { if (l) std::memcpy(body,filename,l); body[l] = 0; }
      return p;