    template<typename t1, typename t2>
    CImg<intT> _patchmatch(const CImg<T>& patch_image,
                           const unsigned int patch_width,
                           const unsigned int patch_height,
                           const unsigned int patch_depth,
                           const unsigned int nb_iterations,
                           const unsigned int nb_randoms,
                           const CImg<t1> &guide,
                           const bool is_matching_score,
                           CImg<t2> &matching_score) const {
      if (is_empty()) return CImg<intT>::const_empty();
      if (patch_image._spectrum!=_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "patchmatch(): Instance image and specified patch image (%u,%u,%u,%u,%p) "
                                    "have different spectrums.",
                                    cimg_instance,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      if (patch_width>_width || patch_height>_height || patch_depth>_depth)
        throw CImgArgumentException(_cimg_instance
                                    "patchmatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the instance image.",
                                    cimg_instance,patch_width,patch_height,patch_depth);
      if (patch_width>patch_image._width || patch_height>patch_image._height || patch_depth>patch_image._depth)
        throw CImgArgumentException(_cimg_instance
                                    "patchmatch(): Specified patch size %ux%ux%u is bigger than the dimensions "
                                    "of the patch image image (%u,%u,%u,%u,%p).",
                                    cimg_instance,patch_width,patch_height,patch_depth,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);
      const unsigned int
        _constraint = patch_image._depth>1?3:2,
        constraint = guide._spectrum>_constraint?_constraint:0;

      if (guide &&
          (guide._width!=_width || guide._height!=_height || guide._depth!=_depth || guide._spectrum<_constraint))
        throw CImgArgumentException(_cimg_instance
                                    "patchmatch(): Specified guide (%u,%u,%u,%u,%p) has invalid dimensions "
                                    "considering instance and patch image image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data,
                                    patch_image._width,patch_image._height,patch_image._depth,patch_image._spectrum,
                                    patch_image._data);

      CImg<intT> map(_width,_height,_depth,patch_image._depth>1?3:2);
      CImg<floatT> score(_width,_height,_depth);
      const int
        psizew = (int)patch_width, psizew1 = psizew/2, psizew2 = psizew - psizew1 - 1,
        psizeh = (int)patch_height, psizeh1 = psizeh/2, psizeh2 = psizeh - psizeh1 - 1,
        psized = (int)patch_depth, psized1 = psized/2, psized2 = psized - psized1 - 1;

      if (_depth>1 || patch_image._depth>1) { // 3d version.

        // Initialize correspondence map.
        if (guide) cimg_forXYZ(*this,x,y,z) { // User-defined initialization.
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              cz1 = z<=psized1?z:(z<depth() - psized2?psized1:psized + z - depth()), cz2 = psized - cz1 - 1,
              u = std::min(std::max((int)guide(x,y,z,0),cx1),patch_image.width() - 1 - cx2),
              v = std::min(std::max((int)guide(x,y,z,1),cy1),patch_image.height() - 1 - cy2),
              w = std::min(std::max((int)guide(x,y,z,2),cz1),patch_image.depth() - 1 - cz2);
            map(x,y,z,0) = u;
            map(x,y,z,1) = v;
            map(x,y,z,2) = w;
            score(x,y,z) = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                       x - cx1,y - cy1,z - cz1,
                                       u - cx1,v - cy1,w - cz1,cimg::type<float>::inf());
          } else cimg_forXYZ(*this,x,y,z) { // Random initialization.
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              cz1 = z<=psized1?z:(z<depth() - psized2?psized1:psized + z - depth()), cz2 = psized - cz1 - 1,
              u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2)),
              v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2)),
              w = (int)cimg::round(cimg::rand(cz1,patch_image.depth() - 1 - cz2));
            map(x,y,z,0) = u;
            map(x,y,z,1) = v;
            map(x,y,z,2) = w;
            score(x,y,z) = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                       x - cx1,y - cy1,z - cz1,
                                       u - cx1,v - cy1,w - cz1,cimg::type<float>::inf());
          }

        // Start iteration loop.
        cimg_abort_init;
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          cimg_abort_test;
          const bool is_even = !(iter%2);

          cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(_width>64 && iter<nb_iterations-2))
          cimg_forXYZ(*this,X,Y,Z) {
            const int
              x = is_even?X:width() - 1 - X,
              y = is_even?Y:height() - 1 - Y,
              z = is_even?Z:depth() - 1 - Z;
            if (score(x,y,z)<=1e-5 || (constraint && guide(x,y,z,constraint)!=0)) continue;
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()), cy2 = psizeh - cy1 - 1,
              cz1 = z<=psized1?z:(z<depth() - psized2?psized1:psized + z - depth()), cz2 = psized - cz1 - 1,
              xp = x - cx1,
              yp = y - cy1,
              zp = z - cz1;

            // Propagation.
            if (is_even) {
              if (x>0) { // Compare with left neighbor.
                const int u = map(x - 1,y,z,0), v = map(x - 1,y,z,1), w = map(x - 1,y,z,2);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u + 1 - cx1,v - cy1,w - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u + 1; map(x,y,z,1) = v; map(x,y,z,2) = w; }
                }
              }
              if (y>0) { // Compare with up neighbor.
                const int u = map(x,y - 1,z,0), v = map(x,y - 1,z,1), w = map(x,y - 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cx2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u - cx1,v + 1 - cy1,w - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u; map(x,y,z,1) = v + 1; map(x,y,z,2) = w; }
                }
              }
              if (z>0) { // Compare with backward neighbor.
                const int u = map(x,y,z - 1,0), v = map(x,y,z - 1,1), w = map(x,y,z - 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 - 1 && w<patch_image.depth() - 1 - cz2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u - cx1,v - cy1,w + 1 - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u; map(x,y,z,1) = v; map(x,y,z,2) = w + 1; }
                }
              }
            } else {
              if (x<width() - 1) { // Compare with right neighbor.
                const int u = map(x + 1,y,z,0), v = map(x + 1,y,z,1), w = map(x + 1,y,z,2);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u - 1 - cx1,v - cy1,w - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u - 1; map(x,y,z,1) = v; map(x,y,z,2) = w; }
                }
              }
              if (y<height() - 1) { // Compare with bottom neighbor.
                const int u = map(x,y + 1,z,0), v = map(x,y + 1,z,1), w = map(x,y + 1,z,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2 &&
                    w>=cz1 && w<patch_image.depth() - cz2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u - cx1,v - 1 - cy1,w - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u; map(x,y,z,1) = v - 1; map(x,y,z,2) = w; }
                }
              }
              if (z<depth() - 1) { // Compare with forward neighbor.
                const int u = map(x,y,z + 1,0), v = map(x,y,z + 1,1), w = map(x,y,z + 1,2);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2 &&
                    w>=cz1 + 1 && w<patch_image.depth() + 1 - cz2) {
                  const float
                    current_score = score(x,y,z),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                    xp,yp,zp,u - cx1,v - cy1,w - 1 - cz1,current_score);
                  if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = u; map(x,y,z,1) = v; map(x,y,z,2) = w - 1; }
                }
              }
            }

            // Randomization.
            const int u = map(x,y,z,0), v = map(x,y,z,1), w = map(x,y,z,2);
            float dw = (float)patch_image.width(), dh = (float)patch_image.height(), dd = (float)patch_image.depth();
            for (unsigned int i = 0; i<nb_randoms; ++i) {
              const int
                ui = (int)cimg::round(cimg::rand(std::max((float)cx1,u - dw),
                                                 std::min(patch_image.width() - 1.0f - cx2,u + dw))),
                vi = (int)cimg::round(cimg::rand(std::max((float)cy1,v - dh),
                                                 std::min(patch_image.height() - 1.0f - cy2,v + dh))),
                wi = (int)cimg::round(cimg::rand(std::max((float)cz1,w - dd),
                                                 std::min(patch_image.depth() - 1.0f - cz2,w + dd)));
              if (ui!=u || vi!=v || wi!=w) {
                const float
                  current_score = score(x,y,z),
                  D = _patchmatch(*this,patch_image,patch_width,patch_height,patch_depth,
                                  xp,yp,zp,ui - cx1,vi - cy1,wi - cz1,current_score);
                if (D<current_score) { score(x,y,z) = D; map(x,y,z,0) = ui; map(x,y,z,1) = vi; map(x,y,z,2) = wi; }
                dw = std::max(5.0f,dw*0.5f); dh = std::max(5.0f,dh*0.5f); dd = std::max(5.0f,dd*0.5f);
              }
            }
          }
        }

      } else { // 2d version.

        // Initialize correspondence map.
        if (guide) cimg_forXY(*this,x,y) { // Random initialization.
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()) , cy2 = psizeh - cy1 - 1,
              u = std::min(std::max((int)guide(x,y,0),cx1),patch_image.width() - 1 - cx2),
              v = std::min(std::max((int)guide(x,y,1),cy1),patch_image.height() - 1 - cy2);
            map(x,y,0) = u;
            map(x,y,1) = v;
            score(x,y) = _patchmatch(*this,patch_image,patch_width,patch_height,
                                     x - cx1,y - cy1,u - cx1,v - cy1,cimg::type<float>::inf());
          } else cimg_forXY(*this,x,y) { // Random initialization.
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()) , cy2 = psizeh - cy1 - 1,
              u = (int)cimg::round(cimg::rand(cx1,patch_image.width() - 1 - cx2)),
              v = (int)cimg::round(cimg::rand(cy1,patch_image.height() - 1 - cy2));
            map(x,y,0) = u;
            map(x,y,1) = v;
            score(x,y) = _patchmatch(*this,patch_image,patch_width,patch_height,
                                     x - cx1,y - cy1,u - cx1,v - cy1,cimg::type<float>::inf());
          }

        // Start iteration loop.
        for (unsigned int iter = 0; iter<nb_iterations; ++iter) {
          const bool is_even = !(iter%2);

          cimg_pragma_openmp(parallel for cimg_openmp_if(_width>64 && iter<nb_iterations-2))
          cimg_forXY(*this,X,Y) {
            const int
              x = is_even?X:width() - 1 - X,
              y = is_even?Y:height() - 1 - Y;
            if (score(x,y)<=1e-5 || (constraint && guide(x,y,constraint)!=0)) continue;
            const int
              cx1 = x<=psizew1?x:(x<width() - psizew2?psizew1:psizew + x - width()), cx2 = psizew - cx1 - 1,
              cy1 = y<=psizeh1?y:(y<height() - psizeh2?psizeh1:psizeh + y - height()) , cy2 = psizeh - cy1 - 1,
              xp = x - cx1,
              yp = y - cy1;

            // Propagation.
            if (is_even) {
              if (x>0) { // Compare with left neighbor.
                const int u = map(x - 1,y,0), v = map(x - 1,y,1);
                if (u>=cx1 - 1 && u<patch_image.width() - 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  const float
                    current_score = score(x,y),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,
                                    xp,yp,u + 1 - cx1,v - cy1,current_score);
                  if (D<current_score) { score(x,y) = D; map(x,y,0) = u + 1; map(x,y,1) = v; }
                }
              }
              if (y>0) { // Compare with up neighbor.
                const int u = map(x,y - 1,0), v = map(x,y - 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 - 1 && v<patch_image.height() - 1 - cy2) {
                  const float
                    current_score = score(x,y),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,
                                    xp,yp,u - cx1,v + 1 - cy1,current_score);
                  if (D<current_score) { score(x,y) = D; map(x,y,0) = u; map(x,y,1) = v + 1; }
                }
              }
            } else {
              if (x<width() - 1) { // Compare with right neighbor.
                const int u = map(x + 1,y,0), v = map(x + 1,y,1);
                if (u>=cx1 + 1 && u<patch_image.width() + 1 - cx2 &&
                    v>=cy1 && v<patch_image.height() - cy2) {
                  const float
                    current_score = score(x,y),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,
                                    xp,yp,u - 1 - cx1,v - cy1,current_score);
                  if (D<current_score) { score(x,y) = D; map(x,y,0) = u - 1; map(x,y,1) = v; }
                }
              }
              if (y<height() - 1) { // Compare with bottom neighbor.
                const int u = map(x,y + 1,0), v = map(x,y + 1,1);
                if (u>=cx1 && u<patch_image.width() - cx2 &&
                    v>=cy1 + 1 && v<patch_image.height() + 1 - cy2) {
                  const float
                    current_score = score(x,y),
                    D = _patchmatch(*this,patch_image,patch_width,patch_height,
                                    xp,yp,u - cx1,v - 1 - cy1,current_score);
                  if (D<current_score) { score(x,y) = D; map(x,y,0) = u; map(x,y,1) = v - 1; }
                }
              }
            }

            // Randomization.
            const int u = map(x,y,0), v = map(x,y,1);
            float dw = (float)patch_image.width(), dh = (float)patch_image.height();
            for (unsigned int i = 0; i<nb_randoms; ++i) {
              const int
                ui = (int)cimg::round(cimg::rand(std::max((float)cx1,u - dw),
                                                 std::min(patch_image.width() - 1.0f - cx2,u + dw))),
                vi = (int)cimg::round(cimg::rand(std::max((float)cy1,v - dh),
                                                 std::min(patch_image.height() - 1.0f - cy2,v + dh)));
              if (ui!=u || vi!=v) {
                const float
                  current_score = score(x,y),
                  D = _patchmatch(*this,patch_image,patch_width,patch_height,
                                  xp,yp,ui - cx1,vi - cy1,current_score);
                if (D<current_score) { score(x,y) = D; map(x,y,0) = ui; map(x,y,1) = vi; }
                dw = std::max(5.0f,dw*0.5f); dh = std::max(5.0f,dh*0.5f);
              }
            }
          }
        }
      }
      if (is_matching_score) score.move_to(matching_score);
      return map;