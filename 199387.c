
    inline unsigned int srand() {
      unsigned int t = (unsigned int)cimg::time();
#if cimg_OS==1
      t+=(unsigned int)getpid();
#elif cimg_OS==2
      t+=(unsigned int)_getpid();
#endif
      return cimg::_rand(t,true);