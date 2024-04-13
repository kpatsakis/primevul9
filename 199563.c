    **/
    CImg<T>& invert(const bool use_LU=true) {
      if (_width!=_height || _depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "invert(): Instance is not a square matrix.",
                                    cimg_instance);
#ifdef cimg_use_lapack
      int INFO = (int)use_LU, N = _width, LWORK = 4*N, *const IPIV = new int[N];
      Tfloat
        *const lapA = new Tfloat[N*N],
        *const WORK = new Tfloat[LWORK];
      cimg_forXY(*this,k,l) lapA[k*N + l] = (Tfloat)((*this)(k,l));
      cimg::getrf(N,lapA,IPIV,INFO);
      if (INFO)
        cimg::warn(_cimg_instance
                   "invert(): LAPACK function dgetrf_() returned error code %d.",
                   cimg_instance,
                   INFO);
      else {
        cimg::getri(N,lapA,IPIV,WORK,LWORK,INFO);
        if (INFO)
          cimg::warn(_cimg_instance
                     "invert(): LAPACK function dgetri_() returned error code %d.",
                     cimg_instance,
                     INFO);
      }
      if (!INFO) cimg_forXY(*this,k,l) (*this)(k,l) = (T)(lapA[k*N + l]); else fill(0);
      delete[] IPIV; delete[] lapA; delete[] WORK;
#else
      const double dete = _width>3?-1.0:det();
      if (dete!=0.0 && _width==2) {
        const double
          a = _data[0], c = _data[1],
          b = _data[2], d = _data[3];
        _data[0] = (T)(d/dete); _data[1] = (T)(-c/dete);
        _data[2] = (T)(-b/dete); _data[3] = (T)(a/dete);
      } else if (dete!=0.0 && _width==3) {
        const double
          a = _data[0], d = _data[1], g = _data[2],
          b = _data[3], e = _data[4], h = _data[5],
          c = _data[6], f = _data[7], i = _data[8];
        _data[0] = (T)((i*e - f*h)/dete), _data[1] = (T)((g*f - i*d)/dete), _data[2] = (T)((d*h - g*e)/dete);
        _data[3] = (T)((h*c - i*b)/dete), _data[4] = (T)((i*a - c*g)/dete), _data[5] = (T)((g*b - a*h)/dete);
        _data[6] = (T)((b*f - e*c)/dete), _data[7] = (T)((d*c - a*f)/dete), _data[8] = (T)((a*e - d*b)/dete);
      } else {
        if (use_LU) { // LU-based inverse computation
          CImg<Tfloat> A(*this,false), indx, col(1,_width);
          bool d;
          A._LU(indx,d);
          cimg_forX(*this,j) {
            col.fill(0);
            col(j) = 1;
            col._solve(A,indx);
            cimg_forX(*this,i) (*this)(j,i) = (T)col(i);
          }
        } else { // SVD-based inverse computation
          CImg<Tfloat> U(_width,_width), S(1,_width), V(_width,_width);
          SVD(U,S,V,false);
          U.transpose();
          cimg_forY(S,k) if (S[k]!=0) S[k]=1/S[k];
          S.diagonal();
          *this = V*S*U;
        }
      }
#endif
      return *this;