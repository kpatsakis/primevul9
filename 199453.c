    // Return a random filename.
    inline const char* filenamerand() {
      cimg::mutex(6);
      static char randomid[9];
      cimg::srand();
      for (unsigned int k = 0; k<8; ++k) {
        const int v = (int)cimg::rand(65535)%3;
        randomid[k] = (char)(v==0?('0' + ((int)cimg::rand(65535)%10)):
                             (v==1?('a' + ((int)cimg::rand(65535)%26)):
                              ('A' + ((int)cimg::rand(65535)%26))));
      }
      cimg::mutex(6,0);
      return randomid;