    **/
    CImg<T>& ror(const char *const expression) {
      return ror((+*this)._fill(expression,true,true,0,0,"ror",this));