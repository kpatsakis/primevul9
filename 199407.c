    template<typename t>
    const CImg<T>& symmetric_eigen(CImg<t>& val, CImg<t>& vec) const {
      if (is_empty()) { val.assign(); vec.assign(); }
      else {
#ifdef cimg_use_lapack
        char JOB = 'V', UPLO = 'U';
        int N = _width, LWORK = 4*N, INFO;
        Tfloat
          *const lapA = new Tfloat[N*N],
          *const lapW = new Tfloat[N],
          *const WORK = new Tfloat[LWORK];
        cimg_forXY(*this,k,l) lapA[k*N + l] = (Tfloat)((*this)(k,l));
        cimg::syev(JOB,UPLO,N,lapA,lapW,WORK,LWORK,INFO);
        if (INFO)
          cimg::warn(_cimg_instance
                     "symmetric_eigen(): LAPACK library function dsyev_() returned error code %d.",
                     cimg_instance,
                     INFO);

        val.assign(1,N);
	vec.assign(N,N);
        if (!INFO) {
          cimg_forY(val,i) val(i) = (T)lapW[N - 1 -i];
          cimg_forXY(vec,k,l) vec(k,l) = (T)(lapA[(N - 1 - k)*N + l]);
        } else { val.fill(0); vec.fill(0); }
        delete[] lapA; delete[] lapW; delete[] WORK;
#else
        if (_width!=_height || _depth>1 || _spectrum>1)
          throw CImgInstanceException(_cimg_instance
                                      "eigen(): Instance is not a square matrix.",
                                      cimg_instance);

	val.assign(1,_width);
	if (vec._data) vec.assign(_width,_width);
        if (_width<3) {
          eigen(val,vec);
          if (_width==2) { vec[1] = -vec[2]; vec[3] = vec[0]; } // Force orthogonality for 2x2 matrices.
          return *this;
        }
        CImg<t> V(_width,_width);
        Tfloat M = 0, m = (Tfloat)min_max(M), maxabs = cimg::max((Tfloat)1,cimg::abs(m),cimg::abs(M));
        (CImg<Tfloat>(*this,false)/=maxabs).SVD(vec,val,V,false);
        if (maxabs!=1) val*=maxabs;

	bool is_ambiguous = false;
	float eig = 0;
	cimg_forY(val,p) {       // check for ambiguous cases.
	  if (val[p]>eig) eig = (float)val[p];
          t scal = 0;
          cimg_forY(vec,y) scal+=vec(p,y)*V(p,y);
          if (cimg::abs(scal)<0.9f) is_ambiguous = true;
          if (scal<0) val[p] = -val[p];
        }
	if (is_ambiguous) {
	  ++(eig*=2);
	  SVD(vec,val,V,false,40,eig);
	  val-=eig;
	}
        CImg<intT> permutations;  // sort eigenvalues in decreasing order
        CImg<t> tmp(_width);
        val.sort(permutations,false);
        cimg_forY(vec,k) {
          cimg_forY(permutations,y) tmp(y) = vec(permutations(y),k);
          std::memcpy(vec.data(0,k),tmp._data,sizeof(t)*_width);
        }
#endif
      }
      return *this;