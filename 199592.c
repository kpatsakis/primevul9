    //! Convert ascii character to upper case.
    inline char uppercase(const char x) {
      return (char)((x<'a'||x>'z')?x:x - 'a' + 'A');