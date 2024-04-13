      // Tell for each character of an expression if it is inside a string or not.
      CImg<boolT> is_inside_string(CImg<charT>& expr) const {
        bool is_escaped = false, next_is_escaped = false;
        unsigned int mode = 0, next_mode = 0; // { 0=normal | 1=char-string | 2=vector-string
        CImg<boolT> res = CImg<charT>::string(expr);
        bool *pd = res._data;
        for (const char *ps = expr._data; *ps; ++ps) {
          if (!next_is_escaped && *ps=='\\') next_is_escaped = true;
          if (!is_escaped && *ps=='\'') { // Non-escaped character
            if (!mode && ps>expr._data && *(ps - 1)=='[') next_mode = mode = 2; // Start vector-string
            else if (mode==2 && *(ps + 1)==']') next_mode = !mode; // End vector-string
            else if (mode<2) next_mode = mode?(mode = 0):1; // Start/end char-string
          }
          *(pd++) = mode>=1 || is_escaped;
          mode = next_mode;
          is_escaped = next_is_escaped;
          next_is_escaped = false;
        }
        return res;