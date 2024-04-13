    template<typename t>
    CImg<T>& solve(const CImg<t>& A) {
      if (_depth!=1 || _spectrum!=1 || _height!=A._height || A._depth!=1 || A._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "solve(): Instance and specified matrix (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    A._width,A._height,A._depth,A._spectrum,A._data);
      typedef _cimg_Ttfloat Ttfloat;
      if (A._width==A._height) { // Classical linear system
        if (_width!=1) {
          CImg<T> res(_width,A._width);
          cimg_forX(*this,i) res.draw_image(i,get_column(i).solve(A));
          return res.move_to(*this);
        }
#ifdef cimg_use_lapack
        char TRANS = 'N';
        int INFO, N = _height, LWORK = 4*N, *const IPIV = new int[N];
        Ttfloat
          *const lapA = new Ttfloat[N*N],
          *const lapB = new Ttfloat[N],
          *const WORK = new Ttfloat[LWORK];
        cimg_forXY(A,k,l) lapA[k*N + l] = (Ttfloat)(A(k,l));
        cimg_forY(*this,i) lapB[i] = (Ttfloat)((*this)(i));
        cimg::getrf(N,lapA,IPIV,INFO);
        if (INFO)
          cimg::warn(_cimg_instance
                     "solve(): LAPACK library function dgetrf_() returned error code %d.",
                     cimg_instance,
                     INFO);

        if (!INFO) {
          cimg::getrs(TRANS,N,lapA,IPIV,lapB,INFO);
          if (INFO)
            cimg::warn(_cimg_instance
                       "solve(): LAPACK library function dgetrs_() returned error code %d.",
                       cimg_instance,
                       INFO);
        }
        if (!INFO) cimg_forY(*this,i) (*this)(i) = (T)(lapB[i]); else fill(0);
        delete[] IPIV; delete[] lapA; delete[] lapB; delete[] WORK;
#else
        CImg<Ttfloat> lu(A,false);
        CImg<Ttfloat> indx;
        bool d;
        lu._LU(indx,d);
        _solve(lu,indx);
#endif
      } else { // Least-square solution for non-square systems.
#ifdef cimg_use_lapack
        if (_width!=1) {
          CImg<T> res(_width,A._width);
          cimg_forX(*this,i) res.draw_image(i,get_column(i).solve(A));
          return res.move_to(*this);
        }
	char TRANS = 'N';
        int INFO, N = A._width, M = A._height, LWORK = -1, LDA = M, LDB = M, NRHS = _width;
	Ttfloat WORK_QUERY;
	Ttfloat
	  * const lapA = new Ttfloat[M*N],
	  * const lapB = new Ttfloat[M*NRHS];
	cimg::sgels(TRANS, M, N, NRHS, lapA, LDA, lapB, LDB, &WORK_QUERY, LWORK, INFO);
	LWORK = (int) WORK_QUERY;
	Ttfloat *const WORK = new Ttfloat[LWORK];
        cimg_forXY(A,k,l) lapA[k*M + l] = (Ttfloat)(A(k,l));
        cimg_forXY(*this,k,l) lapB[k*M + l] = (Ttfloat)((*this)(k,l));
	cimg::sgels(TRANS, M, N, NRHS, lapA, LDA, lapB, LDB, WORK, LWORK, INFO);
        if (INFO != 0)
          cimg::warn(_cimg_instance
                     "solve(): LAPACK library function sgels() returned error code %d.",
                     cimg_instance,
                     INFO);
	assign(NRHS, N);
        if (!INFO)
          cimg_forXY(*this,k,l) (*this)(k,l) = (T)lapB[k*M + l];
        else
          assign(A.get_pseudoinvert()*(*this));
        delete[] lapA; delete[] lapB; delete[] WORK;
#else
	assign(A.get_pseudoinvert()*(*this));
#endif
      }
      return *this;