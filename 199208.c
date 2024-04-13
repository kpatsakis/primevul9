    **/
    inline double grand() {
      double x1, w;
      do {
        const double x2 = cimg::rand(-1,1);
        x1 = cimg::rand(-1,1);
        w = x1*x1 + x2*x2;
      } while (w<=0 || w>=1.0);
      return x1*std::sqrt((-2*std::log(w))/w);