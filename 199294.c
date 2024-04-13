    **/
    inline cimg_long wait(const unsigned int milliseconds) {
      cimg::mutex(3);
      static cimg_ulong timer = 0;
      if (!timer) timer = cimg::time();
      cimg::mutex(3,0);
      return _wait(milliseconds,timer);