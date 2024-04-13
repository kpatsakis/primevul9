    **/
    CImgDisplay& wait(const unsigned int milliseconds) {
      cimg::_wait(milliseconds,_timer);
      return *this;