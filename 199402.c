    template<typename t>
    static int _isosurface3d_indice(const unsigned int edge, const CImg<t>& indices1, const CImg<t>& indices2,
                                    const unsigned int x, const unsigned int y,
                                    const unsigned int nx, const unsigned int ny) {
      switch (edge) {
      case 0 : return indices1(x,y,0);
      case 1 : return indices1(nx,y,1);
      case 2 : return indices1(x,ny,0);
      case 3 : return indices1(x,y,1);
      case 4 : return indices2(x,y,0);
      case 5 : return indices2(nx,y,1);
      case 6 : return indices2(x,ny,0);
      case 7 : return indices2(x,y,1);
      case 8 : return indices1(x,y,2);
      case 9 : return indices1(nx,y,2);
      case 10 : return indices1(nx,ny,2);
      case 11 : return indices1(x,ny,2);
      }
      return 0;