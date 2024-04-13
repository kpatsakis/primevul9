    **/
    CImg<T>& operator=(const char *const expression) {
      const unsigned int omode = cimg::exception_mode();
      cimg::exception_mode(0);
      try {
        _fill(expression,true,true,0,0,"operator=",0);
      } catch (CImgException&) {
        cimg::exception_mode(omode);
        load(expression);
      }
      cimg::exception_mode(omode);
      return *this;