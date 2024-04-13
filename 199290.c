    template<typename tf, typename tp, typename tff>
    CImg<T>& append_object3d(CImgList<tf>& primitives, const CImg<tp>& obj_vertices,
                             const CImgList<tff>& obj_primitives) {
      if (!obj_vertices || !obj_primitives) return *this;
      if (obj_vertices._height!=3 || obj_vertices._depth>1 || obj_vertices._spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "append_object3d(): Specified vertice image (%u,%u,%u,%u,%p) is not a "
                                    "set of 3d vertices.",
                                    cimg_instance,
                                    obj_vertices._width,obj_vertices._height,
                                    obj_vertices._depth,obj_vertices._spectrum,obj_vertices._data);

      if (is_empty()) { primitives.assign(obj_primitives); return assign(obj_vertices); }
      if (_height!=3 || _depth>1 || _spectrum>1)
        throw CImgInstanceException(_cimg_instance
                                    "append_object3d(): Instance is not a set of 3d vertices.",
                                    cimg_instance);

      const unsigned int P = _width;
      append(obj_vertices,'x');
      const unsigned int N = primitives._width;
      primitives.insert(obj_primitives);
      for (unsigned int i = N; i<primitives._width; ++i) {
        CImg<tf> &p = primitives[i];
        switch (p.size()) {
        case 1 : p[0]+=P; break; // Point.
        case 5 : p[0]+=P; p[1]+=P; break; // Sphere.
        case 2 : case 6 : p[0]+=P; p[1]+=P; break; // Segment.
        case 3 : case 9 : p[0]+=P; p[1]+=P; p[2]+=P; break; // Triangle.
        case 4 : case 12 : p[0]+=P; p[1]+=P; p[2]+=P; p[3]+=P; break; // Rectangle.
        }
      }
      return *this;