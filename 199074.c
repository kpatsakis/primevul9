    **/
    inline cimg_int64 fsize(const char *const filename) {
      std::FILE *const file = std::fopen(filename,"rb");
      if (!file) return (cimg_int64)-1;
      std::fseek(file,0,SEEK_END);
      const cimg_int64 siz = (cimg_int64)std::ftell(file);
      std::fclose(file);
      return siz;