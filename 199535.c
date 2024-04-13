     **/
    CImg<T>& rand(const T& val_min, const T& val_max) {
      const float delta = (float)val_max - (float)val_min + (cimg::type<T>::is_float()?0:1);
      if (cimg::type<T>::is_float()) cimg_for(*this,ptrd,T) *ptrd = (T)(val_min + cimg::rand()*delta);
      else cimg_for(*this,ptrd,T) *ptrd = std::min(val_max,(T)(val_min + cimg::rand()*delta));
      return *this;