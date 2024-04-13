    **/
    CImg<T>& max(const char *const expression) {
      return max((+*this)._fill(expression,true,true,0,0,"max",this));