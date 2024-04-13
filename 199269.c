    **/
    CImg<T>& min(const char *const expression) {
      return min((+*this)._fill(expression,true,true,0,0,"min",this));