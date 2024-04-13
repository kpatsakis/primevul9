    */
    static void _cimg_recursive_apply(T *data, const double filter[], const int N, const ulongT off,
				      const unsigned int order, const bool boundary_conditions) {
      double val[4] = { 0 };  // res[n,n - 1,n - 2,n - 3,..] or res[n,n + 1,n + 2,n + 3,..]
      const double
	sumsq = filter[0], sum = sumsq * sumsq,
	a1 = filter[1], a2 = filter[2], a3 = filter[3],
	scaleM = 1.0 / ( (1.0 + a1 - a2 + a3) * (1.0 - a1 - a2 - a3) * (1.0 + a2 + (a1 - a3) * a3) );
      double M[9]; // Triggs matrix
      M[0] = scaleM * (-a3 * a1 + 1.0 - a3 * a3 - a2);
      M[1] = scaleM * (a3 + a1) * (a2 + a3 * a1);
      M[2] = scaleM * a3 * (a1 + a3 * a2);
      M[3] = scaleM * (a1 + a3 * a2);
      M[4] = -scaleM * (a2 - 1.0) * (a2 + a3 * a1);
      M[5] = -scaleM * a3 * (a3 * a1 + a3 * a3 + a2 - 1.0);
      M[6] = scaleM * (a3 * a1 + a2 + a1 * a1 - a2 * a2);
      M[7] = scaleM * (a1 * a2 + a3 * a2 * a2 - a1 * a3 * a3 - a3 * a3 * a3 - a3 * a2 + a3);
      M[8] = scaleM * a3 * (a1 + a3 * a2);
      switch (order) {
      case 0 : {
	const double iplus = (boundary_conditions?data[(N - 1)*off]:(T)0);
	for (int pass = 0; pass<2; ++pass) {
	  if (!pass) {
	    for (int k = 1; k<4; ++k) val[k] = (boundary_conditions?*data/sumsq:0);
	  } else {
	    // apply Triggs boundary conditions
	    const double
	      uplus = iplus/(1.0 - a1 - a2 - a3), vplus = uplus/(1.0 - a1 - a2 - a3),
	      unp  = val[1] - uplus, unp1 = val[2] - uplus, unp2 = val[3] - uplus;
	    val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2 + vplus) * sum;
	    val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2 + vplus) * sum;
	    val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2 + vplus) * sum;
	    *data = (T)val[0];
	    data -= off;
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  for (int n = pass; n<N; ++n) {
	    val[0] = (*data);
	    if (pass) val[0] *= sum;
	    for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
	    *data = (T)val[0];
	    if (!pass) data += off; else data -= off;
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  if (!pass) data -= off;
	}
      } break;
      case 1 : {
	double x[3]; // [front,center,back]
	for (int pass = 0; pass<2; ++pass) {
	  if (!pass) {
	    for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
	    for (int k = 0; k<4; ++k) val[k] = 0;
	  } else {
	    // apply Triggs boundary conditions
	    const double
	      unp  = val[1], unp1 = val[2], unp2 = val[3];
	    val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
	    val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
	    val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
	    *data = (T)val[0];
	    data -= off;
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  for (int n = pass; n<N - 1; ++n) {
	    if (!pass) {
	      x[0] = *(data + off);
	      val[0] = 0.5f * (x[0] - x[2]);
	    } else val[0] = (*data) * sum;
	    for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
	    *data = (T)val[0];
	    if (!pass) {
	      data += off;
	      for (int k = 2; k>0; --k) x[k] = x[k - 1];
	    } else { data-=off;}
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  *data = (T)0;
	}
      } break;
      case 2: {
	double x[3]; // [front,center,back]
	for (int pass = 0; pass<2; ++pass) {
	  if (!pass) {
	    for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
	    for (int k = 0; k<4; ++k) val[k] = 0;
	  } else {
	    // apply Triggs boundary conditions
	    const double
	      unp  = val[1], unp1 = val[2], unp2 = val[3];
	    val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
	    val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
	    val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
	    *data = (T)val[0];
	    data -= off;
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  for (int n = pass; n<N - 1; ++n) {
	    if (!pass) { x[0] = *(data + off); val[0] = (x[1] - x[2]); }
	    else { x[0] = *(data - off); val[0] = (x[2] - x[1]) * sum; }
	    for (int k = 1; k<4; ++k) val[0] += val[k]*filter[k];
	    *data = (T)val[0];
	    if (!pass) data += off; else data -= off;
	    for (int k = 2; k>0; --k) x[k] = x[k - 1];
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  *data = (T)0;
	}
      } break;
      case 3: {
	double x[3]; // [front,center,back]
	for (int pass = 0; pass<2; ++pass) {
	  if (!pass) {
	    for (int k = 0; k<3; ++k) x[k] = (boundary_conditions?*data:(T)0);
	    for (int k = 0; k<4; ++k) val[k] = 0;
	  } else {
	    // apply Triggs boundary conditions
	    const double
	      unp = val[1], unp1 = val[2], unp2 = val[3];
	    val[0] = (M[0] * unp + M[1] * unp1 + M[2] * unp2) * sum;
	    val[1] = (M[3] * unp + M[4] * unp1 + M[5] * unp2) * sum;
	    val[2] = (M[6] * unp + M[7] * unp1 + M[8] * unp2) * sum;
	    *data = (T)val[0];
	    data -= off;
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  for (int n = pass; n<N - 1; ++n) {
	    if (!pass) { x[0] = *(data + off); val[0] = (x[0] - 2*x[1] + x[2]); }
	    else { x[0] = *(data - off); val[0] = 0.5f * (x[2] - x[0]) * sum; }
	    for (int k = 1; k<4; ++k) val[0] += val[k] * filter[k];
	    *data = (T)val[0];
	    if (!pass) data += off; else data -= off;
	    for (int k = 2; k>0; --k) x[k] = x[k - 1];
	    for (int k = 3; k>0; --k) val[k] = val[k - 1];
	  }
	  *data = (T)0;
	}
      } break;
      }