    //! Remove white spaces on the start and/or end of a C-string.
    inline bool strpare(char *const str, const bool is_symmetric, const bool is_iterative) {
      if (!str) return false;
      const int l = (int)std::strlen(str);
      int p, q;
      if (is_symmetric) for (p = 0, q = l - 1; p<q && (signed char)str[p]<=' ' && (signed char)str[q]<=' '; ) {
          --q; ++p; if (!is_iterative) break;
        } else {
        for (p = 0; p<l && (signed char)str[p]<=' '; ) { ++p; if (!is_iterative) break; }
        for (q = l - 1; q>p && (signed char)str[q]<=' '; ) { --q; if (!is_iterative) break; }
      }
      const int n = q - p + 1;
      if (n!=l) { std::memmove(str,str + p,(unsigned int)n); str[n] = 0; return true; }
      return false;