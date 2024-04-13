    template<typename tf>
    static CImg<floatT> cone3d(CImgList<tf>& primitives,
                               const float radius=50, const float size_z=100, const unsigned int subdivisions=24) {
      primitives.assign();
      if (!subdivisions) return CImg<floatT>();
      CImgList<floatT> vertices(2,1,3,1,1,
                                0.,0.,size_z,
                                0.,0.,0.);
      for (float delta = 360.0f/subdivisions, angle = 0; angle<360; angle+=delta) {
	const float a = (float)(angle*cimg::PI/180);
        CImg<floatT>::vector((float)(radius*std::cos(a)),(float)(radius*std::sin(a)),0).move_to(vertices);
      }
      const unsigned int nbr = vertices._width - 2;
      for (unsigned int p = 0; p<nbr; ++p) {
	const unsigned int curr = 2 + p, next = 2 + ((p + 1)%nbr);
        CImg<tf>::vector(1,next,curr).move_to(primitives);
        CImg<tf>::vector(0,curr,next).move_to(primitives);
      }
      return vertices>'x';