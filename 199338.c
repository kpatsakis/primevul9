    template<typename t>
    CImg<T>& distance(const T& value, const CImg<t>& metric_mask) {
      if (is_empty()) return *this;
      bool is_value = false;
      cimg_for(*this,ptr,T) *ptr = *ptr==value?is_value=true,0:(T)999999999;
      if (!is_value) return fill(cimg::type<T>::max());
      const ulongT wh = (ulongT)_width*_height;
      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2))
      cimg_forC(*this,c) {
        CImg<T> img = get_shared_channel(c);
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width*_height*_depth>=1024))
        cimg_forXYZ(metric_mask,dx,dy,dz) {
          const t weight = metric_mask(dx,dy,dz);
          if (weight) {
            for (int z = dz, nz = 0; z<depth(); ++z,++nz) { // Forward scan.
              for (int y = dy , ny = 0; y<height(); ++y,++ny) {
                for (int x = dx, nx = 0; x<width(); ++x,++nx) {
                  const T dd = img(nx,ny,nz,0,wh) + weight;
                  if (dd<img(x,y,z,0,wh)) img(x,y,z,0,wh) = dd;
                }
              }
            }
            for (int z = depth() - 1 - dz, nz = depth() - 1; z>=0; --z,--nz) { // Backward scan.
              for (int y = height() - 1 - dy, ny = height() - 1; y>=0; --y,--ny) {
                for (int x = width() - 1 - dx, nx = width() - 1; x>=0; --x,--nx) {
                  const T dd = img(nx,ny,nz,0,wh) + weight;
                  if (dd<img(x,y,z,0,wh)) img(x,y,z,0,wh) = dd;
                }
              }
            }
          }
        }
      }
      return *this;