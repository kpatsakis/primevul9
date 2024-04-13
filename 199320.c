    template<typename tf, typename t>
    static CImg<floatT> ellipsoid3d(CImgList<tf>& primitives,
                                    const CImg<t>& tensor, const unsigned int subdivisions=3) {
      primitives.assign();
      if (!subdivisions) return CImg<floatT>();
      CImg<floatT> S, V;
      tensor.symmetric_eigen(S,V);
      const float orient =
        (V(0,1)*V(1,2) - V(0,2)*V(1,1))*V(2,0) +
        (V(0,2)*V(1,0) - V(0,0)*V(1,2))*V(2,1) +
        (V(0,0)*V(1,1) - V(0,1)*V(1,0))*V(2,2);
      if (orient<0) { V(2,0) = -V(2,0); V(2,1) = -V(2,1); V(2,2) = -V(2,2); }
      const float l0 = S[0], l1 = S[1], l2 = S[2];
      CImg<floatT> vertices = sphere3d(primitives,1.0,subdivisions);
      vertices.get_shared_row(0)*=l0;
      vertices.get_shared_row(1)*=l1;
      vertices.get_shared_row(2)*=l2;
      return V*vertices;