    **/
    CImg<T>& vanvliet(const float sigma, const unsigned int order, const char axis='x',
                      const bool boundary_conditions=true) {
      if (is_empty()) return *this;
      if (!cimg::type<T>::is_float())
        return CImg<Tfloat>(*this,false).vanvliet(sigma,order,axis,boundary_conditions).move_to(*this);
      const char naxis = cimg::lowercase(axis);
      const float nsigma = sigma>=0?sigma:-sigma*(naxis=='x'?_width:naxis=='y'?_height:naxis=='z'?_depth:_spectrum)/100;
      if (is_empty() || (nsigma<0.5f && !order)) return *this;
      const double
	nnsigma = nsigma<0.5f?0.5f:nsigma,
	m0 = 1.16680, m1 = 1.10783, m2 = 1.40586,
        m1sq = m1 * m1, m2sq = m2 * m2,
	q = (nnsigma<3.556?-0.2568 + 0.5784*nnsigma + 0.0561*nnsigma*nnsigma:2.5091 + 0.9804*(nnsigma - 3.556)),
	qsq = q * q,
	scale = (m0 + q) * (m1sq + m2sq + 2 * m1 * q + qsq),
	b1 = -q * (2 * m0 * m1 + m1sq + m2sq + (2 * m0 + 4 * m1) * q + 3 * qsq) / scale,
	b2 = qsq * (m0 + 2 * m1 + 3 * q) / scale,
	b3 = -qsq * q / scale,
	B = ( m0 * (m1sq + m2sq) ) / scale;
      double filter[4];
      filter[0] = B; filter[1] = -b1; filter[2] = -b2; filter[3] = -b3;
      switch (naxis) {
      case 'x' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
	cimg_forYZC(*this,y,z,c)
	  _cimg_recursive_apply(data(0,y,z,c),filter,_width,1U,order,boundary_conditions);
      } break;
      case 'y' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
	cimg_forXZC(*this,x,z,c)
	  _cimg_recursive_apply(data(x,0,z,c),filter,_height,(ulongT)_width,order,boundary_conditions);
      } break;
      case 'z' : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
	cimg_forXYC(*this,x,y,c)
	  _cimg_recursive_apply(data(x,y,0,c),filter,_depth,(ulongT)_width*_height,
				order,boundary_conditions);
      } break;
      default : {
        cimg_pragma_openmp(parallel for collapse(3) cimg_openmp_if(_width>=256 && _height*_depth*_spectrum>=16))
	cimg_forXYZ(*this,x,y,z)
	  _cimg_recursive_apply(data(x,y,z,0),filter,_spectrum,(ulongT)_width*_height*_depth,
				order,boundary_conditions);
      }
      }
      return *this;