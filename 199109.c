    template<typename t>
    static int _isoline3d_indice(const unsigned int edge, const CImg<t>& indices1, const CImg<t>& indices2,
                                 const unsigned int x, const unsigned int nx) {
      switch (edge) {
      case 0 : return (int)indices1(x,0);
      case 1 : return (int)indices1(nx,1);
      case 2 : return (int)indices2(x,0);
      case 3 : return (int)indices1(x,1);
      }
      return 0;