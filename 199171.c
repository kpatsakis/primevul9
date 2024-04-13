    template<typename t, typename ti>
    CImg<T>& _solve(const CImg<t>& A, const CImg<ti>& indx) {
      typedef _cimg_Ttfloat Ttfloat;
      const int N = (int)size();
      int ii = -1;
      Ttfloat sum;
      for (int i = 0; i<N; ++i) {
        const int ip = (int)indx[i];
        Ttfloat sum = (*this)(ip);
        (*this)(ip) = (*this)(i);
        if (ii>=0) for (int j = ii; j<=i - 1; ++j) sum-=A(j,i)*(*this)(j);
        else if (sum!=0) ii = i;
        (*this)(i) = (T)sum;
      }
      for (int i = N - 1; i>=0; --i) {
        sum = (*this)(i);
        for (int j = i + 1; j<N; ++j) sum-=A(j,i)*(*this)(j);
        (*this)(i) = (T)(sum/A(i,i));
      }
      return *this;