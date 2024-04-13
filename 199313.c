    // Use the system RNG.
    inline unsigned int srand() {
      const unsigned int t = (unsigned int)cimg::time();
#if cimg_OS==1 || defined(__BORLANDC__)
      std::srand(t + (unsigned int)getpid());
#elif cimg_OS==2
      std::srand(t + (unsigned int)_getpid());
#else
      std::srand(t);
#endif
      return t;