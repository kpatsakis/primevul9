    template<typename t>
    CImg<T>& watershed(const CImg<t>& priority, const bool is_high_connectivity=false) {
#define _cimg_watershed_init(cond,X,Y,Z) \
      if (cond && !(*this)(X,Y,Z)) Q._priority_queue_insert(labels,sizeQ,priority(X,Y,Z),X,Y,Z,nb_seeds)

#define _cimg_watershed_propagate(cond,X,Y,Z) \
      if (cond) { \
        if ((*this)(X,Y,Z)) { \
          ns = labels(X,Y,Z) - 1; xs = seeds(ns,0); ys = seeds(ns,1); zs = seeds(ns,2); \
          d = cimg::sqr((float)x - xs) + cimg::sqr((float)y - ys) + cimg::sqr((float)z - zs); \
          if (d<dmin) { dmin = d; nmin = ns; label = (*this)(xs,ys,zs); } \
        } else Q._priority_queue_insert(labels,sizeQ,priority(X,Y,Z),X,Y,Z,n); \
      }

      if (is_empty()) return *this;
      if (!is_sameXYZ(priority))
        throw CImgArgumentException(_cimg_instance
                                    "watershed(): image instance and specified priority (%u,%u,%u,%u,%p) "
                                    "have different dimensions.",
                                    cimg_instance,
                                    priority._width,priority._height,priority._depth,priority._spectrum,priority._data);
      if (_spectrum!=1) {
        cimg_forC(*this,c)
          get_shared_channel(c).watershed(priority.get_shared_channel(c%priority._spectrum));
        return *this;
      }

      CImg<uintT> labels(_width,_height,_depth,1,0), seeds(64,3);
      CImg<typename cimg::superset2<T,t,int>::type> Q;
      unsigned int sizeQ = 0;
      int px, nx, py, ny, pz, nz;
      bool is_px, is_nx, is_py, is_ny, is_pz, is_nz;
      const bool is_3d = _depth>1;

      // Find seed points and insert them in priority queue.
      unsigned int nb_seeds = 0;
      const T *ptrs = _data;
      cimg_forXYZ(*this,x,y,z) if (*(ptrs++)) { // 3d version
        if (nb_seeds>=seeds._width) seeds.resize(2*seeds._width,3,1,1,0);
        seeds(nb_seeds,0) = x; seeds(nb_seeds,1) = y; seeds(nb_seeds++,2) = z;
        px = x - 1; nx = x + 1;
        py = y - 1; ny = y + 1;
        pz = z - 1; nz = z + 1;
        is_px = px>=0; is_nx = nx<width();
        is_py = py>=0; is_ny = ny<height();
        is_pz = pz>=0; is_nz = nz<depth();
        _cimg_watershed_init(is_px,px,y,z);
        _cimg_watershed_init(is_nx,nx,y,z);
        _cimg_watershed_init(is_py,x,py,z);
        _cimg_watershed_init(is_ny,x,ny,z);
        if (is_3d) {
          _cimg_watershed_init(is_pz,x,y,pz);
          _cimg_watershed_init(is_nz,x,y,nz);
        }
        if (is_high_connectivity) {
          _cimg_watershed_init(is_px && is_py,px,py,z);
          _cimg_watershed_init(is_nx && is_py,nx,py,z);
          _cimg_watershed_init(is_px && is_ny,px,ny,z);
          _cimg_watershed_init(is_nx && is_ny,nx,ny,z);
          if (is_3d) {
            _cimg_watershed_init(is_px && is_pz,px,y,pz);
            _cimg_watershed_init(is_nx && is_pz,nx,y,pz);
            _cimg_watershed_init(is_px && is_nz,px,y,nz);
            _cimg_watershed_init(is_nx && is_nz,nx,y,nz);
            _cimg_watershed_init(is_py && is_pz,x,py,pz);
            _cimg_watershed_init(is_ny && is_pz,x,ny,pz);
            _cimg_watershed_init(is_py && is_nz,x,py,nz);
            _cimg_watershed_init(is_ny && is_nz,x,ny,nz);
            _cimg_watershed_init(is_px && is_py && is_pz,px,py,pz);
            _cimg_watershed_init(is_nx && is_py && is_pz,nx,py,pz);
            _cimg_watershed_init(is_px && is_ny && is_pz,px,ny,pz);
            _cimg_watershed_init(is_nx && is_ny && is_pz,nx,ny,pz);
            _cimg_watershed_init(is_px && is_py && is_nz,px,py,nz);
            _cimg_watershed_init(is_nx && is_py && is_nz,nx,py,nz);
            _cimg_watershed_init(is_px && is_ny && is_nz,px,ny,nz);
            _cimg_watershed_init(is_nx && is_ny && is_nz,nx,ny,nz);
          }
        }
        labels(x,y,z) = nb_seeds;
      }

      // Start watershed computation.
      while (sizeQ) {

        // Get and remove point with maximal priority from the queue.
        const int x = (int)Q(0,1), y = (int)Q(0,2), z = (int)Q(0,3);
        const unsigned int n = labels(x,y,z);
        px = x - 1; nx = x + 1;
        py = y - 1; ny = y + 1;
        pz = z - 1; nz = z + 1;
        is_px = px>=0; is_nx = nx<width();
        is_py = py>=0; is_ny = ny<height();
        is_pz = pz>=0; is_nz = nz<depth();

        // Check labels of the neighbors.
        Q._priority_queue_remove(sizeQ);

        unsigned int xs, ys, zs, ns, nmin = 0;
        float d, dmin = cimg::type<float>::inf();
        T label = (T)0;
        _cimg_watershed_propagate(is_px,px,y,z);
        _cimg_watershed_propagate(is_nx,nx,y,z);
        _cimg_watershed_propagate(is_py,x,py,z);
        _cimg_watershed_propagate(is_ny,x,ny,z);
        if (is_3d) {
          _cimg_watershed_propagate(is_pz,x,y,pz);
          _cimg_watershed_propagate(is_nz,x,y,nz);
        }
        if (is_high_connectivity) {
          _cimg_watershed_propagate(is_px && is_py,px,py,z);
          _cimg_watershed_propagate(is_nx && is_py,nx,py,z);
          _cimg_watershed_propagate(is_px && is_ny,px,ny,z);
          _cimg_watershed_propagate(is_nx && is_ny,nx,ny,z);
          if (is_3d) {
            _cimg_watershed_propagate(is_px && is_pz,px,y,pz);
            _cimg_watershed_propagate(is_nx && is_pz,nx,y,pz);
            _cimg_watershed_propagate(is_px && is_nz,px,y,nz);
            _cimg_watershed_propagate(is_nx && is_nz,nx,y,nz);
            _cimg_watershed_propagate(is_py && is_pz,x,py,pz);
            _cimg_watershed_propagate(is_ny && is_pz,x,ny,pz);
            _cimg_watershed_propagate(is_py && is_nz,x,py,nz);
            _cimg_watershed_propagate(is_ny && is_nz,x,ny,nz);
            _cimg_watershed_propagate(is_px && is_py && is_pz,px,py,pz);
            _cimg_watershed_propagate(is_nx && is_py && is_pz,nx,py,pz);
            _cimg_watershed_propagate(is_px && is_ny && is_pz,px,ny,pz);
            _cimg_watershed_propagate(is_nx && is_ny && is_pz,nx,ny,pz);
            _cimg_watershed_propagate(is_px && is_py && is_nz,px,py,nz);
            _cimg_watershed_propagate(is_nx && is_py && is_nz,nx,py,nz);
            _cimg_watershed_propagate(is_px && is_ny && is_nz,px,ny,nz);
            _cimg_watershed_propagate(is_nx && is_ny && is_nz,nx,ny,nz);
          }
        }
        (*this)(x,y,z) = label;
        labels(x,y,z) = ++nmin;
      }
      return *this;