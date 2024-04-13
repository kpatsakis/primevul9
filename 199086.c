    template<typename tf>
    static CImg<floatT> sphere3d(CImgList<tf>& primitives,
                                 const float radius=50, const unsigned int subdivisions=3) {

      // Create initial icosahedron
      primitives.assign();
      const double tmp = (1 + std::sqrt(5.0f))/2, a = 1.0/std::sqrt(1 + tmp*tmp), b = tmp*a;
      CImgList<floatT> vertices(12,1,3,1,1, b,a,0.0, -b,a,0.0, -b,-a,0.0, b,-a,0.0, a,0.0,b, a,0.0,-b,
                                -a,0.0,-b, -a,0.0,b, 0.0,b,a, 0.0,-b,a, 0.0,-b,-a, 0.0,b,-a);
      primitives.assign(20,1,3,1,1, 4,8,7, 4,7,9, 5,6,11, 5,10,6, 0,4,3, 0,3,5, 2,7,1, 2,1,6,
                        8,0,11, 8,11,1, 9,10,3, 9,2,10, 8,4,0, 11,0,5, 4,9,3,
                        5,3,10, 7,8,1, 6,1,11, 7,2,9, 6,10,2);
      // edge - length/2
      float he = (float)a;

      // Recurse subdivisions
      for (unsigned int i = 0; i<subdivisions; ++i) {
        const unsigned int L = primitives._width;
	he/=2;
	const float he2 = he*he;
        for (unsigned int l = 0; l<L; ++l) {
          const unsigned int
            p0 = (unsigned int)primitives(0,0), p1 = (unsigned int)primitives(0,1), p2 = (unsigned int)primitives(0,2);
          const float
            x0 = vertices(p0,0), y0 = vertices(p0,1), z0 = vertices(p0,2),
            x1 = vertices(p1,0), y1 = vertices(p1,1), z1 = vertices(p1,2),
            x2 = vertices(p2,0), y2 = vertices(p2,1), z2 = vertices(p2,2),
            tnx0 = (x0 + x1)/2, tny0 = (y0 + y1)/2, tnz0 = (z0 + z1)/2,
            nn0 = cimg::hypot(tnx0,tny0,tnz0),
            tnx1 = (x0 + x2)/2, tny1 = (y0 + y2)/2, tnz1 = (z0 + z2)/2,
            nn1 = cimg::hypot(tnx1,tny1,tnz1),
            tnx2 = (x1 + x2)/2, tny2 = (y1 + y2)/2, tnz2 = (z1 + z2)/2,
            nn2 = cimg::hypot(tnx2,tny2,tnz2),
            nx0 = tnx0/nn0, ny0 = tny0/nn0, nz0 = tnz0/nn0,
            nx1 = tnx1/nn1, ny1 = tny1/nn1, nz1 = tnz1/nn1,
            nx2 = tnx2/nn2, ny2 = tny2/nn2, nz2 = tnz2/nn2;
          int i0 = -1, i1 = -1, i2 = -1;
          cimglist_for(vertices,p) {
            const float x = (float)vertices(p,0), y = (float)vertices(p,1), z = (float)vertices(p,2);
            if (cimg::sqr(x-nx0) + cimg::sqr(y-ny0) + cimg::sqr(z-nz0)<he2) i0 = p;
            if (cimg::sqr(x-nx1) + cimg::sqr(y-ny1) + cimg::sqr(z-nz1)<he2) i1 = p;
            if (cimg::sqr(x-nx2) + cimg::sqr(y-ny2) + cimg::sqr(z-nz2)<he2) i2 = p;
          }
          if (i0<0) { CImg<floatT>::vector(nx0,ny0,nz0).move_to(vertices); i0 = vertices.width() - 1; }
          if (i1<0) { CImg<floatT>::vector(nx1,ny1,nz1).move_to(vertices); i1 = vertices.width() - 1; }
          if (i2<0) { CImg<floatT>::vector(nx2,ny2,nz2).move_to(vertices); i2 = vertices.width() - 1; }
          primitives.remove(0);
          CImg<tf>::vector(p0,i0,i1).move_to(primitives);
          CImg<tf>::vector((tf)i0,(tf)p1,(tf)i2).move_to(primitives);
          CImg<tf>::vector((tf)i1,(tf)i2,(tf)p2).move_to(primitives);
          CImg<tf>::vector((tf)i1,(tf)i0,(tf)i2).move_to(primitives);
        }
      }
      return (vertices>'x')*=radius;