     **/
    CImg<T>& operator/=(const char *const expression) {
      return div((+*this)._fill(expression,true,true,0,0,"operator/=",this));