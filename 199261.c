
      static double mp_list_ixyzc(_cimg_math_parser& mp) {
        const unsigned int
          ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listin.width()),
          interpolation = (unsigned int)_mp_arg(7),
          boundary_conditions = (unsigned int)_mp_arg(8);
        const CImg<T> &img = mp.listin[ind];
        const double
          x = _mp_arg(3), y = _mp_arg(4),
          z = _mp_arg(5), c = _mp_arg(6);
        if (interpolation==0) switch (boundary_conditions) { // Nearest neighbor interpolation
          case 3 : { // Mirror
            const int
              w2 = 2*img.width(), h2 = 2*img.height(), d2 = 2*img.depth(), s2 = 2*img.spectrum(),
              mx = cimg::mod((int)x,w2), my = cimg::mod((int)y,h2),
              mz = cimg::mod((int)z,d2), mc = cimg::mod((int)c,s2);
            return (double)img(mx<img.width()?mx:w2 - mx - 1,
                               my<img.height()?my:h2 - my - 1,
                               mz<img.depth()?mz:d2 - mz - 1,
                               mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img(cimg::mod((int)x,img.width()),
                               cimg::mod((int)y,img.height()),
                               cimg::mod((int)z,img.depth()),
                               cimg::mod((int)c,img.spectrum()));
          case 1 : // Neumann
            return (double)img._atXYZC((int)x,(int)y,(int)z,(int)c);
          default : // Dirichlet
            return (double)img.atXYZC((int)x,(int)y,(int)z,(int)c,(T)0);
          } else switch (boundary_conditions) { // Linear interpolation
          case 3 : { // Mirror
            const float
              w2 = 2.0f*img.width(), h2 = 2.0f*img.height(), d2 = 2.0f*img.depth(), s2 = 2.0f*img.spectrum(),
              mx = cimg::mod((float)x,w2), my = cimg::mod((float)y,h2),
              mz = cimg::mod((float)z,d2), mc = cimg::mod((float)c,s2);
            return (double)img._linear_atXYZC(mx<img.width()?mx:w2 - mx - 1,
                                              my<img.height()?my:h2 - my - 1,
                                              mz<img.depth()?mz:d2 - mz - 1,
                                              mc<img.spectrum()?mc:s2 - mc - 1);
          }
          case 2 : // Periodic
            return (double)img._linear_atXYZC(cimg::mod((float)x,(float)img.width()),
                                              cimg::mod((float)y,(float)img.height()),
                                              cimg::mod((float)z,(float)img.depth()),
                                              cimg::mod((float)c,(float)img.spectrum()));
          case 1 : // Neumann
            return (double)img._linear_atXYZC((float)x,(float)y,(float)z,(float)c);
          default : // Dirichlet
            return (double)img.linear_atXYZC((float)x,(float)y,(float)z,(float)c,(T)0);
          }