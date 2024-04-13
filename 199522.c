    template<typename tf, typename tfunc>
    static CImg<floatT> isoline3d(CImgList<tf>& primitives, const tfunc& func, const float isovalue,
                                  const float x0, const float y0, const float x1, const float y1,
                                  const int size_x=256, const int size_y=256) {
      static const unsigned int edges[16] = { 0x0, 0x9, 0x3, 0xa, 0x6, 0xf, 0x5, 0xc, 0xc,
                                              0x5, 0xf, 0x6, 0xa, 0x3, 0x9, 0x0 };
      static const int segments[16][4] = { { -1,-1,-1,-1 }, { 0,3,-1,-1 }, { 0,1,-1,-1 }, { 1,3,-1,-1 },
                                           { 1,2,-1,-1 },   { 0,1,2,3 },   { 0,2,-1,-1 }, { 2,3,-1,-1 },
                                           { 2,3,-1,-1 },   { 0,2,-1,-1},  { 0,3,1,2 },   { 1,2,-1,-1 },
                                           { 1,3,-1,-1 },   { 0,1,-1,-1},  { 0,3,-1,-1},  { -1,-1,-1,-1 } };
      const unsigned int
        _nx = (unsigned int)(size_x>=0?size_x:cimg::round((x1-x0)*-size_x/100 + 1)),
        _ny = (unsigned int)(size_y>=0?size_y:cimg::round((y1-y0)*-size_y/100 + 1)),
        nx = _nx?_nx:1,
        ny = _ny?_ny:1,
        nxm1 = nx - 1,
        nym1 = ny - 1;
      primitives.assign();
      if (!nxm1 || !nym1) return CImg<floatT>();
      const float dx = (x1 - x0)/nxm1, dy = (y1 - y0)/nym1;
      CImgList<floatT> vertices;
      CImg<intT> indices1(nx,1,1,2,-1), indices2(nx,1,1,2);
      CImg<floatT> values1(nx), values2(nx);
      float X = x0, Y = y0, nX = X + dx, nY = Y + dy;

      // Fill first line with values
      cimg_forX(values1,x) { values1(x) = (float)func(X,Y); X+=dx; }

      // Run the marching squares algorithm
      for (unsigned int yi = 0, nyi = 1; yi<nym1; ++yi, ++nyi, Y=nY, nY+=dy) {
        X = x0; nX = X + dx;
        indices2.fill(-1);
        for (unsigned int xi = 0, nxi = 1; xi<nxm1; ++xi, ++nxi, X=nX, nX+=dx) {

          // Determine square configuration
          const float
            val0 = values1(xi),
            val1 = values1(nxi),
            val2 = values2(nxi) = (float)func(nX,nY),
            val3 = values2(xi) = (float)func(X,nY);
          const unsigned int
            configuration = (val0<isovalue?1U:0U) | (val1<isovalue?2U:0U) |
            (val2<isovalue?4U:0U) | (val3<isovalue?8U:0U),
            edge = edges[configuration];

          // Compute intersection vertices
          if (edge) {
            if ((edge&1) && indices1(xi,0)<0) {
              const float Xi = X + (isovalue-val0)*dx/(val1-val0);
              indices1(xi,0) = vertices.width();
              CImg<floatT>::vector(Xi,Y,0).move_to(vertices);
            }
            if ((edge&2) && indices1(nxi,1)<0) {
              const float Yi = Y + (isovalue-val1)*dy/(val2-val1);
              indices1(nxi,1) = vertices.width();
              CImg<floatT>::vector(nX,Yi,0).move_to(vertices);
            }
            if ((edge&4) && indices2(xi,0)<0) {
              const float Xi = X + (isovalue-val3)*dx/(val2-val3);
              indices2(xi,0) = vertices.width();
              CImg<floatT>::vector(Xi,nY,0).move_to(vertices);
            }
            if ((edge&8) && indices1(xi,1)<0) {
              const float Yi = Y + (isovalue-val0)*dy/(val3-val0);
              indices1(xi,1) = vertices.width();
              CImg<floatT>::vector(X,Yi,0).move_to(vertices);
            }

            // Create segments
            for (const int *segment = segments[configuration]; *segment!=-1; ) {
              const unsigned int p0 = (unsigned int)*(segment++), p1 = (unsigned int)*(segment++);
              const tf
                i0 = (tf)(_isoline3d_indice(p0,indices1,indices2,xi,nxi)),
                i1 = (tf)(_isoline3d_indice(p1,indices1,indices2,xi,nxi));
              CImg<tf>::vector(i0,i1).move_to(primitives);
            }
          }
        }
        values1.swap(values2);
        indices1.swap(indices2);
      }
      return vertices>'x';