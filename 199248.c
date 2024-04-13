    // Locally solve eikonal equation.
    Tfloat __distance_eikonal(const CImg<Tfloat>& res, const Tfloat P,
                              const int x=0, const int y=0, const int z=0) const {
      const Tfloat M = (Tfloat)cimg::type<T>::max();
      T T1 = (T)std::min(x - 1>=0?res(x - 1,y,z):M,x + 1<width()?res(x + 1,y,z):M);
      Tfloat root = 0;
      if (_depth>1) { // 3d.
        T
          T2 = (T)std::min(y - 1>=0?res(x,y - 1,z):M,y + 1<height()?res(x,y + 1,z):M),
          T3 = (T)std::min(z - 1>=0?res(x,y,z - 1):M,z + 1<depth()?res(x,y,z + 1):M);
        if (T1>T2) cimg::swap(T1,T2);
        if (T2>T3) cimg::swap(T2,T3);
        if (T1>T2) cimg::swap(T1,T2);
        if (P<=0) return (Tfloat)T1;
        if (T3<M && ___distance_eikonal(3,-2*(T1 + T2 + T3),T1*T1 + T2*T2 + T3*T3 - P*P,root))
          return std::max((Tfloat)T3,root);
        if (T2<M && ___distance_eikonal(2,-2*(T1 + T2),T1*T1 + T2*T2 - P*P,root))
          return std::max((Tfloat)T2,root);
        return P + T1;
      } else if (_height>1) { // 2d.
        T T2 = (T)std::min(y - 1>=0?res(x,y - 1,z):M,y + 1<height()?res(x,y + 1,z):M);
        if (T1>T2) cimg::swap(T1,T2);
        if (P<=0) return (Tfloat)T1;
        if (T2<M && ___distance_eikonal(2,-2*(T1 + T2),T1*T1 + T2*T2 - P*P,root))
          return std::max((Tfloat)T2,root);
        return P + T1;
      } else { // 1d.
        if (P<=0) return (Tfloat)T1;
        return P + T1;
      }
      return 0;