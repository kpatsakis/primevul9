    // [internal] Perform box filter with dirichlet boundary conditions.
    CImg<T>& _blur_guided(const unsigned int psize) {
      const int p1 = (int)psize/2, p2 = (int)psize - p1;
      if (_depth!=1) {
        CImg<floatT> cumul = get_cumulate('z'), cumul2 = cumul.get_shift(0,0,p2,0,1);
        (cumul.shift(0,0,-p1,0,1)-=cumul2).move_to(*this);
      }
      if (_height!=1) {
        CImg<floatT> cumul = get_cumulate('y'), cumul2 = cumul.get_shift(0,p2,0,0,1);
        (cumul.shift(0,-p1,0,0,1)-=cumul2).move_to(*this);
      }
      if (_width!=1) {
        CImg<floatT> cumul = get_cumulate('x'), cumul2 = cumul.get_shift(p2,0,0,0,1);
        (cumul.shift(-p1,0,0,0,1)-=cumul2).move_to(*this);
      }
      return *this;