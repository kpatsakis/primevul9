    **/
    inline bool is_file(const char *const path) {
      if (!path || !*path) return false;
      std::FILE *const file = std_fopen(path,"rb");
      if (!file) return false;
      std::fclose(file);
      return !is_directory(path);