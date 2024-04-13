    template<typename t>
    CImg<Tfloat> get_distance_eikonal(const T& value, const CImg<t>& metric) const {
      if (is_empty()) return *this;
      if (!is_sameXYZ(metric))
        throw CImgArgumentException(_cimg_instance
                                    "distance_eikonal(): image instance and metric map (%u,%u,%u,%u) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    metric._width,metric._height,metric._depth,metric._spectrum);
      CImg<Tfloat> result(_width,_height,_depth,_spectrum,cimg::type<Tfloat>::max()), Q;
      CImg<charT> state(_width,_height,_depth); // -1=far away, 0=narrow, 1=frozen.

      cimg_pragma_openmp(parallel for cimg_openmp_if(_spectrum>=2) firstprivate(Q,state))
      cimg_forC(*this,c) {
        const CImg<T> img = get_shared_channel(c);
        const CImg<t> met = metric.get_shared_channel(c%metric._spectrum);
        CImg<Tfloat> res = result.get_shared_channel(c);
        unsigned int sizeQ = 0;
        state.fill(-1);

        // Detect initial seeds.
        Tfloat *ptr1 = res._data; char *ptr2 = state._data;
        cimg_for(img,ptr0,T) { if (*ptr0==value) { *ptr1 = 0; *ptr2 = 1; } ++ptr1; ++ptr2; }

        // Initialize seeds neighbors.
        ptr2 = state._data;
        cimg_forXYZ(img,x,y,z) if (*(ptr2++)==1) {
          if (x - 1>=0 && state(x - 1,y,z)==-1) {
            const Tfloat dist = res(x - 1,y,z) = __distance_eikonal(res,met(x - 1,y,z),x - 1,y,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x - 1,y,z);
          }
          if (x + 1<width() && state(x + 1,y,z)==-1) {
            const Tfloat dist = res(x + 1,y,z) = __distance_eikonal(res,met(x + 1,y,z),x + 1,y,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x + 1,y,z);
          }
          if (y - 1>=0 && state(x,y - 1,z)==-1) {
            const Tfloat dist = res(x,y - 1,z) = __distance_eikonal(res,met(x,y - 1,z),x,y - 1,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y - 1,z);
          }
          if (y + 1<height() && state(x,y + 1,z)==-1) {
            const Tfloat dist = res(x,y + 1,z) = __distance_eikonal(res,met(x,y + 1,z),x,y + 1,z);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y + 1,z);
          }
          if (z - 1>=0 && state(x,y,z - 1)==-1) {
            const Tfloat dist = res(x,y,z - 1) = __distance_eikonal(res,met(x,y,z - 1),x,y,z - 1);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z - 1);
          }
          if (z + 1<depth() && state(x,y,z + 1)==-1) {
            const Tfloat dist = res(x,y,z + 1) = __distance_eikonal(res,met(x,y,z + 1),x,y,z + 1);
            Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z + 1);
          }
        }

        // Propagate front.
        while (sizeQ) {
          int x = -1, y = -1, z = -1;
          while (sizeQ && x<0) {
            x = (int)Q(0,1); y = (int)Q(0,2); z = (int)Q(0,3);
            Q._priority_queue_remove(sizeQ);
            if (state(x,y,z)==1) x = -1; else state(x,y,z) = 1;
          }
          if (x>=0) {
            if (x - 1>=0 && state(x - 1,y,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x - 1,y,z),x - 1,y,z);
              if (dist<res(x - 1,y,z)) {
                res(x - 1,y,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x - 1,y,z);
              }
            }
            if (x + 1<width() && state(x + 1,y,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x + 1,y,z),x + 1,y,z);
              if (dist<res(x + 1,y,z)) {
                res(x + 1,y,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x + 1,y,z);
              }
            }
            if (y - 1>=0 && state(x,y - 1,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y - 1,z),x,y - 1,z);
              if (dist<res(x,y - 1,z)) {
                res(x,y - 1,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y - 1,z);
              }
            }
            if (y + 1<height() && state(x,y + 1,z)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y + 1,z),x,y + 1,z);
              if (dist<res(x,y + 1,z)) {
                res(x,y + 1,z) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y + 1,z);
              }
            }
            if (z - 1>=0 && state(x,y,z - 1)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y,z - 1),x,y,z - 1);
              if (dist<res(x,y,z - 1)) {
                res(x,y,z - 1) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z - 1);
              }
            }
            if (z + 1<depth() && state(x,y,z + 1)!=1) {
              const Tfloat dist = __distance_eikonal(res,met(x,y,z + 1),x,y,z + 1);
              if (dist<res(x,y,z + 1)) {
                res(x,y,z + 1) = dist; Q._eik_priority_queue_insert(state,sizeQ,-dist,x,y,z + 1);
              }
            }
          }
        }
      }
      return result;