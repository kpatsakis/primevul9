    template<typename t>
    CImg<T>& _LU(CImg<t>& indx, bool& d) {
      const int N = width();
      int imax = 0;
      CImg<Tfloat> vv(N);
      indx.assign(N);
      d = true;
      cimg_forX(*this,i) {
        Tfloat vmax = 0;
        cimg_forX(*this,j) {
          const Tfloat tmp = cimg::abs((*this)(j,i));
          if (tmp>vmax) vmax = tmp;
        }
        if (vmax==0) { indx.fill(0); return fill(0); }
        vv[i] = 1/vmax;
      }
      cimg_forX(*this,j) {
        for (int i = 0; i<j; ++i) {
          Tfloat sum=(*this)(j,i);
          for (int k = 0; k<i; ++k) sum-=(*this)(k,i)*(*this)(j,k);
          (*this)(j,i) = (T)sum;
        }
        Tfloat vmax = 0;
        for (int i = j; i<width(); ++i) {
          Tfloat sum=(*this)(j,i);
          for (int k = 0; k<j; ++k) sum-=(*this)(k,i)*(*this)(j,k);
          (*this)(j,i) = (T)sum;
          const Tfloat tmp = vv[i]*cimg::abs(sum);
          if (tmp>=vmax) { vmax=tmp; imax=i; }
        }
        if (j!=imax) {
          cimg_forX(*this,k) cimg::swap((*this)(k,imax),(*this)(k,j));
          d =!d;
          vv[imax] = vv[j];
        }
        indx[j] = (t)imax;
        if ((*this)(j,j)==0) (*this)(j,j) = (T)1e-20;
        if (j<N) {
          const Tfloat tmp = 1/(Tfloat)(*this)(j,j);
          for (int i = j + 1; i<N; ++i) (*this)(j,i) = (T)((*this)(j,i)*tmp);
        }
      }
      return *this;