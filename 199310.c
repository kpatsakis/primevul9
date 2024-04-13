    **/
    CImg<T>& rol(const char *const expression) {
      return rol((+*this)._fill(expression,true,true,0,0,"rol",this));