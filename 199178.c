    //! Convert ascii character to lower case.
    inline char lowercase(const char x) {
      return (char)((x<'A'||x>'Z')?x:x - 'A' + 'a');