    template<typename tz, typename tp, typename tf, typename tc, typename to>
    CImg<T>& _draw_object3d(void *const pboard, CImg<tz>& zbuffer,
                            const float X, const float Y, const float Z,
                            const CImg<tp>& vertices,
                            const CImgList<tf>& primitives,
                            const CImgList<tc>& colors,
                            const to& opacities,
                            const unsigned int render_type,
                            const bool is_double_sided, const float focale,
                            const float lightx, const float lighty, const float lightz,
                            const float specular_lightness, const float specular_shininess,
                            const float sprite_scale) {
      typedef typename cimg::superset2<tp,tz,float>::type tpfloat;
      typedef typename to::value_type _to;
      if (is_empty() || !vertices || !primitives) return *this;
      CImg<char> error_message(1024);
      if (!vertices.is_object3d(primitives,colors,opacities,false,error_message))
        throw CImgArgumentException(_cimg_instance
                                    "draw_object3d(): Invalid specified 3d object (%u,%u) (%s).",
                                    cimg_instance,vertices._width,primitives._width,error_message.data());
#ifndef cimg_use_board
      if (pboard) return *this;
#endif
      if (render_type==5) cimg::mutex(10);  // Static variable used in this case, breaks thread-safety.

      const float
        nspec = 1 - (specular_lightness<0.0f?0.0f:(specular_lightness>1.0f?1.0f:specular_lightness)),
        nspec2 = 1 + (specular_shininess<0.0f?0.0f:specular_shininess),
        nsl1 = (nspec2 - 1)/cimg::sqr(nspec - 1),
        nsl2 = 1 - 2*nsl1*nspec,
        nsl3 = nspec2 - nsl1 - nsl2;

      // Create light texture for phong-like rendering.
      CImg<floatT> light_texture;
      if (render_type==5) {
        if (colors._width>primitives._width) {
          static CImg<floatT> default_light_texture;
          static const tc *lptr = 0;
          static tc ref_values[64] = { 0 };
          const CImg<tc>& img = colors.back();
          bool is_same_texture = (lptr==img._data);
          if (is_same_texture)
            for (unsigned int r = 0, j = 0; j<8; ++j)
              for (unsigned int i = 0; i<8; ++i)
                if (ref_values[r++]!=img(i*img._width/9,j*img._height/9,0,(i + j)%img._spectrum)) {
                  is_same_texture = false; break;
                }
          if (!is_same_texture || default_light_texture._spectrum<_spectrum) {
            (default_light_texture.assign(img,false)/=255).resize(-100,-100,1,_spectrum);
            lptr = colors.back().data();
            for (unsigned int r = 0, j = 0; j<8; ++j)
              for (unsigned int i = 0; i<8; ++i)
                ref_values[r++] = img(i*img._width/9,j*img._height/9,0,(i + j)%img._spectrum);
          }
          light_texture.assign(default_light_texture,true);
        } else {
          static CImg<floatT> default_light_texture;
          static float olightx = 0, olighty = 0, olightz = 0, ospecular_shininess = 0;
          if (!default_light_texture ||
              lightx!=olightx || lighty!=olighty || lightz!=olightz ||
              specular_shininess!=ospecular_shininess || default_light_texture._spectrum<_spectrum) {
            default_light_texture.assign(512,512);
            const float
              dlx = lightx - X,
              dly = lighty - Y,
              dlz = lightz - Z,
              nl = cimg::hypot(dlx,dly,dlz),
              nlx = (default_light_texture._width - 1)/2*(1 + dlx/nl),
              nly = (default_light_texture._height - 1)/2*(1 + dly/nl),
              white[] = { 1 };
            default_light_texture.draw_gaussian(nlx,nly,default_light_texture._width/3.0f,white);
            cimg_forXY(default_light_texture,x,y) {
              const float factor = default_light_texture(x,y);
              if (factor>nspec) default_light_texture(x,y) = std::min(2.0f,nsl1*factor*factor + nsl2*factor + nsl3);
            }
            default_light_texture.resize(-100,-100,1,_spectrum);
            olightx = lightx; olighty = lighty; olightz = lightz; ospecular_shininess = specular_shininess;
          }
          light_texture.assign(default_light_texture,true);
        }
      }

      // Compute 3d to 2d projection.
      CImg<tpfloat> projections(vertices._width,2);
      tpfloat parallzmin = cimg::type<tpfloat>::max();
      const float absfocale = focale?cimg::abs(focale):0;
      if (absfocale) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(projections.size()>4096))
        cimg_forX(projections,l) { // Perspective projection
          const tpfloat
            x = (tpfloat)vertices(l,0),
            y = (tpfloat)vertices(l,1),
            z = (tpfloat)vertices(l,2);
          const tpfloat projectedz = z + Z + absfocale;
          projections(l,1) = Y + absfocale*y/projectedz;
          projections(l,0) = X + absfocale*x/projectedz;
        }
      } else {
        cimg_pragma_openmp(parallel for cimg_openmp_if(projections.size()>4096))
        cimg_forX(projections,l) { // Parallel projection
          const tpfloat
            x = (tpfloat)vertices(l,0),
            y = (tpfloat)vertices(l,1),
            z = (tpfloat)vertices(l,2);
          if (z<parallzmin) parallzmin = z;
          projections(l,1) = Y + y;
          projections(l,0) = X + x;
        }
      }

      const float _focale = absfocale?absfocale:(1e5f-parallzmin);
      float zmax = 0;
      if (zbuffer) zmax = vertices.get_shared_row(2).max();

      // Compute visible primitives.
      CImg<uintT> visibles(primitives._width,1,1,1,~0U);
      CImg<tpfloat> zrange(primitives._width);
      const tpfloat zmin = absfocale?(tpfloat)(1.5f - absfocale):cimg::type<tpfloat>::min();
      bool is_forward = zbuffer?true:false;

      cimg_pragma_openmp(parallel for cimg_openmp_if(primitives.size()>4096))
      cimglist_for(primitives,l) {
        const CImg<tf>& primitive = primitives[l];
        switch (primitive.size()) {
        case 1 : { // Point
          CImg<_to> _opacity;
          __draw_object3d(opacities,l,_opacity);
          if (l<=colors.width() && (colors[l].size()!=_spectrum || _opacity)) is_forward = false;
          const unsigned int i0 = (unsigned int)primitive(0);
          const tpfloat z0 = Z + vertices(i0,2);
          if (z0>zmin) {
            visibles(l) = (unsigned int)l;
            zrange(l) = z0;
          }
        } break;
        case 5 : { // Sphere
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          const tpfloat
            Xc = 0.5f*((float)vertices(i0,0) + (float)vertices(i1,0)),
            Yc = 0.5f*((float)vertices(i0,1) + (float)vertices(i1,1)),
            Zc = 0.5f*((float)vertices(i0,2) + (float)vertices(i1,2)),
            _zc = Z + Zc,
            zc = _zc + _focale,
            xc = X + Xc*(absfocale?absfocale/zc:1),
            yc = Y + Yc*(absfocale?absfocale/zc:1),
            radius = 0.5f*cimg::hypot(vertices(i1,0) - vertices(i0,0),
                                      vertices(i1,1) - vertices(i0,1),
                                      vertices(i1,2) - vertices(i0,2))*(absfocale?absfocale/zc:1),
            xm = xc - radius,
            ym = yc - radius,
            xM = xc + radius,
            yM = yc + radius;
          if (xM>=0 && xm<_width && yM>=0 && ym<_height && _zc>zmin) {
            visibles(l) = (unsigned int)l;
            zrange(l) = _zc;
          }
          is_forward = false;
        } break;
        case 2 : // Segment
        case 6 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1);
          const tpfloat
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + vertices(i1,2);
          tpfloat xm, xM, ym, yM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (xM>=0 && xm<_width && yM>=0 && ym<_height && z0>zmin && z1>zmin) {
            visibles(l) = (unsigned int)l;
            zrange(l) = (z0 + z1)/2;
          }
        } break;
        case 3 :  // Triangle
        case 9 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2);
          const tpfloat
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + vertices(i1,2),
            x2 = projections(i2,0), y2 = projections(i2,1), z2 = Z + vertices(i2,2);
          tpfloat xm, xM, ym, yM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (x2<xm) xm = x2;
          if (x2>xM) xM = x2;
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (y2<ym) ym = y2;
          if (y2>yM) yM = y2;
          if (xM>=0 && xm<_width && yM>=0 && ym<_height && z0>zmin && z1>zmin && z2>zmin) {
            const tpfloat d = (x1-x0)*(y2-y0) - (x2-x0)*(y1-y0);
            if (is_double_sided || d<0) {
              visibles(l) = (unsigned int)l;
              zrange(l) = (z0 + z1 + z2)/3;
            }
          }
        } break;
        case 4 : // Rectangle
        case 12 : {
          const unsigned int
            i0 = (unsigned int)primitive(0),
            i1 = (unsigned int)primitive(1),
            i2 = (unsigned int)primitive(2),
            i3 = (unsigned int)primitive(3);
          const tpfloat
            x0 = projections(i0,0), y0 = projections(i0,1), z0 = Z + vertices(i0,2),
            x1 = projections(i1,0), y1 = projections(i1,1), z1 = Z + vertices(i1,2),
            x2 = projections(i2,0), y2 = projections(i2,1), z2 = Z + vertices(i2,2),
            x3 = projections(i3,0), y3 = projections(i3,1), z3 = Z + vertices(i3,2);
          tpfloat xm, xM, ym, yM;
          if (x0<x1) { xm = x0; xM = x1; } else { xm = x1; xM = x0; }
          if (x2<xm) xm = x2;
          if (x2>xM) xM = x2;
          if (x3<xm) xm = x3;
          if (x3>xM) xM = x3;
          if (y0<y1) { ym = y0; yM = y1; } else { ym = y1; yM = y0; }
          if (y2<ym) ym = y2;
          if (y2>yM) yM = y2;
          if (y3<ym) ym = y3;
          if (y3>yM) yM = y3;
          if (xM>=0 && xm<_width && yM>=0 && ym<_height && z0>zmin && z1>zmin && z2>zmin && z3>zmin) {
            const float d = (x1 - x0)*(y2 - y0) - (x2 - x0)*(y1 - y0);
            if (is_double_sided || d<0) {
              visibles(l) = (unsigned int)l;
              zrange(l) = (z0 + z1 + z2 + z3)/4;
            }
          }
        } break;
        default :
          if (render_type==5) cimg::mutex(10,0);
          throw CImgArgumentException(_cimg_instance
                                      "draw_object3d(): Invalid primitive[%u] with size %u "
                                      "(should have size 1,2,3,4,5,6,9 or 12).",
                                      cimg_instance,
                                      l,primitive.size());
        }
      }

      // Force transparent primitives to be drawn last when zbuffer is activated
      // (and if object contains no spheres or sprites).
      if (is_forward)
        cimglist_for(primitives,l)
          if (___draw_object3d(opacities,l)!=1) zrange(l) = 2*zmax - zrange(l);

      // Sort only visibles primitives.
      unsigned int *p_visibles = visibles._data;
      tpfloat *p_zrange = zrange._data;
      const tpfloat *ptrz = p_zrange;
      cimg_for(visibles,ptr,unsigned int) {
        if (*ptr!=~0U) { *(p_visibles++) = *ptr; *(p_zrange++) = *ptrz; }
        ++ptrz;
      }
      const unsigned int nb_visibles = (unsigned int)(p_zrange - zrange._data);
      if (!nb_visibles) {
        if (render_type==5) cimg::mutex(10,0);
        return *this;
      }
      CImg<uintT> permutations;
      CImg<tpfloat>(zrange._data,nb_visibles,1,1,1,true).sort(permutations,is_forward);

      // Compute light properties
      CImg<floatT> lightprops;
      switch (render_type) {
      case 3 : { // Flat Shading
        lightprops.assign(nb_visibles);
        cimg_pragma_openmp(parallel for cimg_openmp_if(nb_visibles>4096))
        cimg_forX(lightprops,l) {
          const CImg<tf>& primitive = primitives(visibles(permutations(l)));
          const unsigned int psize = (unsigned int)primitive.size();
          if (psize==3 || psize==4 || psize==9 || psize==12) {
            const unsigned int
              i0 = (unsigned int)primitive(0),
              i1 = (unsigned int)primitive(1),
              i2 = (unsigned int)primitive(2);
            const tpfloat
              x0 = (tpfloat)vertices(i0,0), y0 = (tpfloat)vertices(i0,1), z0 = (tpfloat)vertices(i0,2),
              x1 = (tpfloat)vertices(i1,0), y1 = (tpfloat)vertices(i1,1), z1 = (tpfloat)vertices(i1,2),
              x2 = (tpfloat)vertices(i2,0), y2 = (tpfloat)vertices(i2,1), z2 = (tpfloat)vertices(i2,2),
              dx1 = x1 - x0, dy1 = y1 - y0, dz1 = z1 - z0,
              dx2 = x2 - x0, dy2 = y2 - y0, dz2 = z2 - z0,
              nx = dy1*dz2 - dz1*dy2,
              ny = dz1*dx2 - dx1*dz2,
              nz = dx1*dy2 - dy1*dx2,
              norm = 1e-5f + cimg::hypot(nx,ny,nz),
              lx = X + (x0 + x1 + x2)/3 - lightx,
              ly = Y + (y0 + y1 + y2)/3 - lighty,
              lz = Z + (z0 + z1 + z2)/3 - lightz,
              nl = 1e-5f + cimg::hypot(lx,ly,lz),
              factor = std::max(cimg::abs(-lx*nx - ly*ny - lz*nz)/(norm*nl),(tpfloat)0);
            lightprops[l] = factor<=nspec?factor:(nsl1*factor*factor + nsl2*factor + nsl3);
          } else lightprops[l] = 1;
        }
      } break;

      case 4 : // Gouraud Shading
      case 5 : { // Phong-Shading
        CImg<tpfloat> vertices_normals(vertices._width,6,1,1,0);
        cimg_pragma_openmp(parallel for cimg_openmp_if(nb_visibles>4096))
        for (unsigned int l = 0; l<nb_visibles; ++l) {
          const CImg<tf>& primitive = primitives[visibles(l)];
          const unsigned int psize = (unsigned int)primitive.size();
          const bool
            triangle_flag = (psize==3) || (psize==9),
            rectangle_flag = (psize==4) || (psize==12);
          if (triangle_flag || rectangle_flag) {
            const unsigned int
              i0 = (unsigned int)primitive(0),
              i1 = (unsigned int)primitive(1),
              i2 = (unsigned int)primitive(2),
              i3 = rectangle_flag?(unsigned int)primitive(3):0;
            const tpfloat
              x0 = (tpfloat)vertices(i0,0), y0 = (tpfloat)vertices(i0,1), z0 = (tpfloat)vertices(i0,2),
              x1 = (tpfloat)vertices(i1,0), y1 = (tpfloat)vertices(i1,1), z1 = (tpfloat)vertices(i1,2),
              x2 = (tpfloat)vertices(i2,0), y2 = (tpfloat)vertices(i2,1), z2 = (tpfloat)vertices(i2,2),
              dx1 = x1 - x0, dy1 = y1 - y0, dz1 = z1 - z0,
              dx2 = x2 - x0, dy2 = y2 - y0, dz2 = z2 - z0,
              nnx = dy1*dz2 - dz1*dy2,
              nny = dz1*dx2 - dx1*dz2,
              nnz = dx1*dy2 - dy1*dx2,
              norm = 1e-5f + cimg::hypot(nnx,nny,nnz),
              nx = nnx/norm,
              ny = nny/norm,
              nz = nnz/norm;
            unsigned int ix = 0, iy = 1, iz = 2;
            if (is_double_sided && nz>0) { ix = 3; iy = 4; iz = 5; }
            vertices_normals(i0,ix)+=nx; vertices_normals(i0,iy)+=ny; vertices_normals(i0,iz)+=nz;
            vertices_normals(i1,ix)+=nx; vertices_normals(i1,iy)+=ny; vertices_normals(i1,iz)+=nz;
            vertices_normals(i2,ix)+=nx; vertices_normals(i2,iy)+=ny; vertices_normals(i2,iz)+=nz;
            if (rectangle_flag) {
              vertices_normals(i3,ix)+=nx; vertices_normals(i3,iy)+=ny; vertices_normals(i3,iz)+=nz;
            }
          }
        }

        if (is_double_sided) cimg_forX(vertices_normals,p) {
            const float
              nx0 = vertices_normals(p,0), ny0 = vertices_normals(p,1), nz0 = vertices_normals(p,2),
              nx1 = vertices_normals(p,3), ny1 = vertices_normals(p,4), nz1 = vertices_normals(p,5),
              n0 = nx0*nx0 + ny0*ny0 + nz0*nz0, n1 = nx1*nx1 + ny1*ny1 + nz1*nz1;
            if (n1>n0) {
              vertices_normals(p,0) = -nx1;
              vertices_normals(p,1) = -ny1;
              vertices_normals(p,2) = -nz1;
            }
          }

        if (render_type==4) {
          lightprops.assign(vertices._width);
          cimg_pragma_openmp(parallel for cimg_openmp_if(nb_visibles>4096))
          cimg_forX(lightprops,l) {
            const tpfloat
              nx = vertices_normals(l,0),
              ny = vertices_normals(l,1),
              nz = vertices_normals(l,2),
              norm = 1e-5f + cimg::hypot(nx,ny,nz),
              lx = X + vertices(l,0) - lightx,
              ly = Y + vertices(l,1) - lighty,
              lz = Z + vertices(l,2) - lightz,
              nl = 1e-5f + cimg::hypot(lx,ly,lz),
              factor = std::max((-lx*nx - ly*ny - lz*nz)/(norm*nl),(tpfloat)0);
            lightprops[l] = factor<=nspec?factor:(nsl1*factor*factor + nsl2*factor + nsl3);
          }
        } else {
          const unsigned int
            lw2 = light_texture._width/2 - 1,
            lh2 = light_texture._height/2 - 1;
          lightprops.assign(vertices._width,2);
          cimg_pragma_openmp(parallel for cimg_openmp_if(nb_visibles>4096))
          cimg_forX(lightprops,l) {
            const tpfloat
              nx = vertices_normals(l,0),
              ny = vertices_normals(l,1),
              nz = vertices_normals(l,2),
              norm = 1e-5f + cimg::hypot(nx,ny,nz),
              nnx = nx/norm,
              nny = ny/norm;
            lightprops(l,0) = lw2*(1 + nnx);
            lightprops(l,1) = lh2*(1 + nny);
          }
        }
      } break;
      }

      // Draw visible primitives
      const CImg<tc> default_color(1,_spectrum,1,1,(tc)200);
      CImg<_to> _opacity;

      for (unsigned int l = 0; l<nb_visibles; ++l) {
        const unsigned int n_primitive = visibles(permutations(l));
        const CImg<tf>& primitive = primitives[n_primitive];
        const CImg<tc>
          &__color = n_primitive<colors._width?colors[n_primitive]:CImg<tc>(),
          _color = (__color && __color.size()!=_spectrum && __color._spectrum<_spectrum)?
            __color.get_resize(-100,-100,-100,_spectrum,0):CImg<tc>(),
          &color = _color?_color:(__color?__color:default_color);
        const tc *const pcolor = color._data;
        const float opacity = __draw_object3d(opacities,n_primitive,_opacity);

#ifdef cimg_use_board
        LibBoard::Board &board = *(LibBoard::Board*)pboard;
#endif

        switch (primitive.size()) {
        case 1 : { // Colored point or sprite
          const unsigned int n0 = (unsigned int)primitive[0];
          const int x0 = (int)projections(n0,0), y0 = (int)projections(n0,1);

          if (_opacity.is_empty()) { // Scalar opacity.

            if (color.size()==_spectrum) { // Colored point.
              draw_point(x0,y0,pcolor,opacity);
#ifdef cimg_use_board
              if (pboard) {
                board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
                board.drawDot((float)x0,height()-(float)y0);
              }
#endif
            } else { // Sprite.
              const tpfloat z = Z + vertices(n0,2);
              const float factor = focale<0?1:sprite_scale*(absfocale?absfocale/(z + absfocale):1);
              const unsigned int
                _sw = (unsigned int)(color._width*factor),
                _sh = (unsigned int)(color._height*factor),
                sw = _sw?_sw:1, sh = _sh?_sh:1;
              const int nx0 = x0 - (int)sw/2, ny0 = y0 - (int)sh/2;
              if (sw<=3*_width/2 && sh<=3*_height/2 &&
                  (nx0 + (int)sw/2>=0 || nx0 - (int)sw/2<width() || ny0 + (int)sh/2>=0 || ny0 - (int)sh/2<height())) {
                const CImg<tc>
                  _sprite = (sw!=color._width || sh!=color._height)?
                    color.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<tc>(),
                  &sprite = _sprite?_sprite:color;
                draw_image(nx0,ny0,sprite,opacity);
#ifdef cimg_use_board
                if (pboard) {
                  board.setPenColorRGBi(128,128,128);
                  board.setFillColor(LibBoard::Color::Null);
                  board.drawRectangle((float)nx0,height() - (float)ny0,sw,sh);
                }
#endif
              }
            }
          } else { // Opacity mask.
            const tpfloat z = Z + vertices(n0,2);
            const float factor = focale<0?1:sprite_scale*(absfocale?absfocale/(z + absfocale):1);
            const unsigned int
              _sw = (unsigned int)(std::max(color._width,_opacity._width)*factor),
              _sh = (unsigned int)(std::max(color._height,_opacity._height)*factor),
              sw = _sw?_sw:1, sh = _sh?_sh:1;
            const int nx0 = x0 - (int)sw/2, ny0 = y0 - (int)sh/2;
            if (sw<=3*_width/2 && sh<=3*_height/2 &&
                (nx0 + (int)sw/2>=0 || nx0 - (int)sw/2<width() || ny0 + (int)sh/2>=0 || ny0 - (int)sh/2<height())) {
              const CImg<tc>
                _sprite = (sw!=color._width || sh!=color._height)?
                  color.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<tc>(),
                &sprite = _sprite?_sprite:color;
              const CImg<_to>
                _nopacity = (sw!=_opacity._width || sh!=_opacity._height)?
                  _opacity.get_resize(sw,sh,1,-100,render_type<=3?1:3):CImg<_to>(),
                &nopacity = _nopacity?_nopacity:_opacity;
              draw_image(nx0,ny0,sprite,nopacity);
#ifdef cimg_use_board
              if (pboard) {
                board.setPenColorRGBi(128,128,128);
                board.setFillColor(LibBoard::Color::Null);
                board.drawRectangle((float)nx0,height() - (float)ny0,sw,sh);
              }
#endif
            }
          }
        } break;
        case 2 : { // Colored line
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1];
          const int
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale;
          if (render_type) {
            if (zbuffer) draw_line(zbuffer,x0,y0,z0,x1,y1,z1,pcolor,opacity);
            else draw_line(x0,y0,x1,y1,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,x1,height() - (float)y1);
            }
#endif
          } else {
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
            }
#endif
          }
        } break;
        case 5 : { // Colored sphere
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            is_wireframe = (unsigned int)primitive[2];
          const float
            Xc = 0.5f*((float)vertices(n0,0) + (float)vertices(n1,0)),
            Yc = 0.5f*((float)vertices(n0,1) + (float)vertices(n1,1)),
            Zc = 0.5f*((float)vertices(n0,2) + (float)vertices(n1,2)),
            zc = Z + Zc + _focale,
            xc = X + Xc*(absfocale?absfocale/zc:1),
            yc = Y + Yc*(absfocale?absfocale/zc:1),
            radius = 0.5f*cimg::hypot(vertices(n1,0) - vertices(n0,0),
                                      vertices(n1,1) - vertices(n0,1),
                                      vertices(n1,2) - vertices(n0,2))*(absfocale?absfocale/zc:1);
          switch (render_type) {
          case 0 :
            draw_point((int)xc,(int)yc,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawDot(xc,height() - yc);
            }
#endif
            break;
          case 1 :
            draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity,~0U);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.setFillColor(LibBoard::Color::Null);
              board.drawCircle(xc,height() - yc,radius);
            }
#endif
            break;
          default :
            if (is_wireframe) draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity,~0U);
            else draw_circle((int)xc,(int)yc,(int)radius,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              if (!is_wireframe) board.fillCircle(xc,height() - yc,radius);
              else {
                board.setFillColor(LibBoard::Color::Null);
                board.drawCircle(xc,height() - yc,radius);
              }
            }
#endif
            break;
          }
        } break;
        case 6 : { // Textured line
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for line primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1];
          const int
            tx0 = (int)primitive[2], ty0 = (int)primitive[3],
            tx1 = (int)primitive[4], ty1 = (int)primitive[5],
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale;
          if (render_type) {
            if (zbuffer) draw_line(zbuffer,x0,y0,z0,x1,y1,z1,color,tx0,ty0,tx1,ty1,opacity);
            else draw_line(x0,y0,x1,y1,color,tx0,ty0,tx1,ty1,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,(float)x1,height() - (float)y1);
            }
#endif
          } else {
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
            }
#endif
          }
        } break;
        case 3 : { // Colored triangle
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2];
          const int
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1),
            x2 = (int)projections(n2,0), y2 = (int)projections(n2,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale,
            z2 = vertices(n2,2) + Z + _focale;
          switch (render_type) {
          case 0 :
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity).draw_point(x2,y2,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
              board.drawDot((float)x2,height() - (float)y2);
            }
#endif
            break;
          case 1 :
            if (zbuffer)
              draw_line(zbuffer,x0,y0,z0,x1,y1,z1,pcolor,opacity).draw_line(zbuffer,x0,y0,z0,x2,y2,z2,pcolor,opacity).
                draw_line(zbuffer,x1,y1,z1,x2,y2,z2,pcolor,opacity);
            else
              draw_line(x0,y0,x1,y1,pcolor,opacity).draw_line(x0,y0,x2,y2,pcolor,opacity).
                draw_line(x1,y1,x2,y2,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,(float)x1,height() - (float)y1);
              board.drawLine((float)x0,height() - (float)y0,(float)x2,height() - (float)y2);
              board.drawLine((float)x1,height() - (float)y1,(float)x2,height() - (float)y2);
            }
#endif
            break;
          case 2 :
            if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,opacity);
            else draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
            }
#endif
            break;
          case 3 :
            if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,opacity,lightprops(l));
            else _draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,opacity,lightprops(l));
#ifdef cimg_use_board
            if (pboard) {
              const float lp = std::min(lightprops(l),1);
              board.setPenColorRGBi((unsigned char)(color[0]*lp),
                                     (unsigned char)(color[1]*lp),
                                     (unsigned char)(color[2]*lp),
                                     (unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
            }
#endif
            break;
          case 4 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,
                            lightprops(n0),lightprops(n1),lightprops(n2),opacity);
            else draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,lightprops(n0),lightprops(n1),lightprops(n2),opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi((unsigned char)(color[0]),
                                     (unsigned char)(color[1]),
                                     (unsigned char)(color[2]),
                                     (unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,lightprops(n0),
                                         (float)x1,height() - (float)y1,lightprops(n1),
                                         (float)x2,height() - (float)y2,lightprops(n2));
            }
#endif
            break;
          case 5 : {
            const unsigned int
              lx0 = (unsigned int)lightprops(n0,0), ly0 = (unsigned int)lightprops(n0,1),
              lx1 = (unsigned int)lightprops(n1,0), ly1 = (unsigned int)lightprops(n1,1),
              lx2 = (unsigned int)lightprops(n2,0), ly2 = (unsigned int)lightprops(n2,1);
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
            else draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
#ifdef cimg_use_board
            if (pboard) {
              const float
                l0 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n0,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n0,1)))),
                l1 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n1,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n1,1)))),
                l2 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n2,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n2,1))));
              board.setPenColorRGBi((unsigned char)(color[0]),
                                     (unsigned char)(color[1]),
                                     (unsigned char)(color[2]),
                                     (unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,l0,
                                         (float)x1,height() - (float)y1,l1,
                                         (float)x2,height() - (float)y2,l2);
            }
#endif
          } break;
          }
        } break;
        case 4 : { // Colored rectangle
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2],
            n3 = (unsigned int)primitive[3];
          const int
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1),
            x2 = (int)projections(n2,0), y2 = (int)projections(n2,1),
            x3 = (int)projections(n3,0), y3 = (int)projections(n3,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale,
            z2 = vertices(n2,2) + Z + _focale,
            z3 = vertices(n3,2) + Z + _focale;

          switch (render_type) {
          case 0 :
            draw_point(x0,y0,pcolor,opacity).draw_point(x1,y1,pcolor,opacity).
              draw_point(x2,y2,pcolor,opacity).draw_point(x3,y3,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
              board.drawDot((float)x2,height() - (float)y2);
              board.drawDot((float)x3,height() - (float)y3);
            }
#endif
            break;
          case 1 :
            if (zbuffer)
              draw_line(zbuffer,x0,y0,z0,x1,y1,z1,pcolor,opacity).draw_line(zbuffer,x1,y1,z1,x2,y2,z2,pcolor,opacity).
                draw_line(zbuffer,x2,y2,z2,x3,y3,z3,pcolor,opacity).draw_line(zbuffer,x3,y3,z3,x0,y0,z0,pcolor,opacity);
            else
              draw_line(x0,y0,x1,y1,pcolor,opacity).draw_line(x1,y1,x2,y2,pcolor,opacity).
                draw_line(x2,y2,x3,y3,pcolor,opacity).draw_line(x3,y3,x0,y0,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,(float)x1,height() - (float)y1);
              board.drawLine((float)x1,height() - (float)y1,(float)x2,height() - (float)y2);
              board.drawLine((float)x2,height() - (float)y2,(float)x3,height() - (float)y3);
              board.drawLine((float)x3,height() - (float)y3,(float)x0,height() - (float)y0);
            }
#endif
            break;
          case 2 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,pcolor,opacity);
            else
              draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,opacity).draw_triangle(x0,y0,x2,y2,x3,y3,pcolor,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(color[0],color[1],color[2],(unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x2,height() - (float)y2,
                                 (float)x3,height() - (float)y3);
            }
#endif
            break;
          case 3 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,opacity,lightprops(l)).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,pcolor,opacity,lightprops(l));
            else
              _draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,opacity,lightprops(l)).
                _draw_triangle(x0,y0,x2,y2,x3,y3,pcolor,opacity,lightprops(l));
#ifdef cimg_use_board
            if (pboard) {
              const float lp = std::min(lightprops(l),1);
              board.setPenColorRGBi((unsigned char)(color[0]*lp),
                                     (unsigned char)(color[1]*lp),
                                     (unsigned char)(color[2]*lp),(unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x2,height() - (float)y2,
                                 (float)x3,height() - (float)y3);
            }
#endif
            break;
          case 4 : {
            const float
              lightprop0 = lightprops(n0), lightprop1 = lightprops(n1),
              lightprop2 = lightprops(n2), lightprop3 = lightprops(n3);
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,lightprop0,lightprop1,lightprop2,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,pcolor,lightprop0,lightprop2,lightprop3,opacity);
            else
              draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,lightprop0,lightprop1,lightprop2,opacity).
                draw_triangle(x0,y0,x2,y2,x3,y3,pcolor,lightprop0,lightprop2,lightprop3,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi((unsigned char)(color[0]),
                                     (unsigned char)(color[1]),
                                     (unsigned char)(color[2]),
                                     (unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,lightprop0,
                                         (float)x1,height() - (float)y1,lightprop1,
                                         (float)x2,height() - (float)y2,lightprop2);
              board.fillGouraudTriangle((float)x0,height() - (float)y0,lightprop0,
                                         (float)x2,height() - (float)y2,lightprop2,
                                         (float)x3,height() - (float)y3,lightprop3);
            }
#endif
          } break;
          case 5 : {
            const unsigned int
              lx0 = (unsigned int)lightprops(n0,0), ly0 = (unsigned int)lightprops(n0,1),
              lx1 = (unsigned int)lightprops(n1,0), ly1 = (unsigned int)lightprops(n1,1),
              lx2 = (unsigned int)lightprops(n2,0), ly2 = (unsigned int)lightprops(n2,1),
              lx3 = (unsigned int)lightprops(n3,0), ly3 = (unsigned int)lightprops(n3,1);
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,pcolor,light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity);
            else
              draw_triangle(x0,y0,x1,y1,x2,y2,pcolor,light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity).
                draw_triangle(x0,y0,x2,y2,x3,y3,pcolor,light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity);
#ifdef cimg_use_board
            if (pboard) {
              const float
                l0 = light_texture((int)(light_texture.width()/2*(1 + lx0)), (int)(light_texture.height()/2*(1 + ly0))),
                l1 = light_texture((int)(light_texture.width()/2*(1 + lx1)), (int)(light_texture.height()/2*(1 + ly1))),
                l2 = light_texture((int)(light_texture.width()/2*(1 + lx2)), (int)(light_texture.height()/2*(1 + ly2))),
                l3 = light_texture((int)(light_texture.width()/2*(1 + lx3)), (int)(light_texture.height()/2*(1 + ly3)));
              board.setPenColorRGBi((unsigned char)(color[0]),
                                     (unsigned char)(color[1]),
                                     (unsigned char)(color[2]),
                                     (unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,l0,
                                         (float)x1,height() - (float)y1,l1,
                                         (float)x2,height() - (float)y2,l2);
              board.fillGouraudTriangle((float)x0,height() - (float)y0,l0,
                                         (float)x2,height() - (float)y2,l2,
                                         (float)x3,height() - (float)y3,l3);
            }
#endif
          } break;
          }
        } break;
        case 9 : { // Textured triangle
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for triangle primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2];
          const int
            tx0 = (int)primitive[3], ty0 = (int)primitive[4],
            tx1 = (int)primitive[5], ty1 = (int)primitive[6],
            tx2 = (int)primitive[7], ty2 = (int)primitive[8],
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1),
            x2 = (int)projections(n2,0), y2 = (int)projections(n2,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale,
            z2 = vertices(n2,2) + Z + _focale;
          switch (render_type) {
          case 0 :
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity).
              draw_point(x2,y2,color.get_vector_at(tx2<=0?0:tx2>=color.width()?color.width() - 1:tx2,
                                                   ty2<=0?0:ty2>=color.height()?color.height() - 1:ty2)._data,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
              board.drawDot((float)x2,height() - (float)y2);
            }
#endif
            break;
          case 1 :
            if (zbuffer)
              draw_line(zbuffer,x0,y0,z0,x1,y1,z1,color,tx0,ty0,tx1,ty1,opacity).
                draw_line(zbuffer,x0,y0,z0,x2,y2,z2,color,tx0,ty0,tx2,ty2,opacity).
                draw_line(zbuffer,x1,y1,z1,x2,y2,z2,color,tx1,ty1,tx2,ty2,opacity);
            else
              draw_line(x0,y0,z0,x1,y1,z1,color,tx0,ty0,tx1,ty1,opacity).
                draw_line(x0,y0,z0,x2,y2,z2,color,tx0,ty0,tx2,ty2,opacity).
                draw_line(x1,y1,z1,x2,y2,z2,color,tx1,ty1,tx2,ty2,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,(float)x1,height() - (float)y1);
              board.drawLine((float)x0,height() - (float)y0,(float)x2,height() - (float)y2);
              board.drawLine((float)x1,height() - (float)y1,(float)x2,height() - (float)y2);
            }
#endif
            break;
          case 2 :
            if (zbuffer) draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity);
            else draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
            }
#endif
            break;
          case 3 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l));
            else draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l));
#ifdef cimg_use_board
            if (pboard) {
              const float lp = std::min(lightprops(l),1);
              board.setPenColorRGBi((unsigned char)(128*lp),
                                    (unsigned char)(128*lp),
                                    (unsigned char)(128*lp),
                                    (unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
            }
#endif
            break;
          case 4 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            lightprops(n0),lightprops(n1),lightprops(n2),opacity);
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            lightprops(n0),lightprops(n1),lightprops(n2),opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,lightprops(n0),
                                        (float)x1,height() - (float)y1,lightprops(n1),
                                        (float)x2,height() - (float)y2,lightprops(n2));
            }
#endif
            break;
          case 5 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,light_texture,
                            (unsigned int)lightprops(n0,0),(unsigned int)lightprops(n0,1),
                            (unsigned int)lightprops(n1,0),(unsigned int)lightprops(n1,1),
                            (unsigned int)lightprops(n2,0),(unsigned int)lightprops(n2,1),
                            opacity);
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,light_texture,
                            (unsigned int)lightprops(n0,0),(unsigned int)lightprops(n0,1),
                            (unsigned int)lightprops(n1,0),(unsigned int)lightprops(n1,1),
                            (unsigned int)lightprops(n2,0),(unsigned int)lightprops(n2,1),
                            opacity);
#ifdef cimg_use_board
            if (pboard) {
              const float
                l0 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n0,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n0,1)))),
                l1 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n1,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n1,1)))),
                l2 = light_texture((int)(light_texture.width()/2*(1 + lightprops(n2,0))),
                                   (int)(light_texture.height()/2*(1 + lightprops(n2,1))));
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,l0,
                                        (float)x1,height() - (float)y1,l1,
                                        (float)x2,height() - (float)y2,l2);
            }
#endif
            break;
          }
        } break;
        case 12 : { // Textured quadrangle
          if (!__color) {
            if (render_type==5) cimg::mutex(10,0);
            throw CImgArgumentException(_cimg_instance
                                        "draw_object3d(): Undefined texture for quadrangle primitive [%u].",
                                        cimg_instance,n_primitive);
          }
          const unsigned int
            n0 = (unsigned int)primitive[0],
            n1 = (unsigned int)primitive[1],
            n2 = (unsigned int)primitive[2],
            n3 = (unsigned int)primitive[3];
          const int
            tx0 = (int)primitive[4], ty0 = (int)primitive[5],
            tx1 = (int)primitive[6], ty1 = (int)primitive[7],
            tx2 = (int)primitive[8], ty2 = (int)primitive[9],
            tx3 = (int)primitive[10], ty3 = (int)primitive[11],
            x0 = (int)projections(n0,0), y0 = (int)projections(n0,1),
            x1 = (int)projections(n1,0), y1 = (int)projections(n1,1),
            x2 = (int)projections(n2,0), y2 = (int)projections(n2,1),
            x3 = (int)projections(n3,0), y3 = (int)projections(n3,1);
          const float
            z0 = vertices(n0,2) + Z + _focale,
            z1 = vertices(n1,2) + Z + _focale,
            z2 = vertices(n2,2) + Z + _focale,
            z3 = vertices(n3,2) + Z + _focale;

          switch (render_type) {
          case 0 :
            draw_point(x0,y0,color.get_vector_at(tx0<=0?0:tx0>=color.width()?color.width() - 1:tx0,
                                                 ty0<=0?0:ty0>=color.height()?color.height() - 1:ty0)._data,opacity).
              draw_point(x1,y1,color.get_vector_at(tx1<=0?0:tx1>=color.width()?color.width() - 1:tx1,
                                                   ty1<=0?0:ty1>=color.height()?color.height() - 1:ty1)._data,opacity).
              draw_point(x2,y2,color.get_vector_at(tx2<=0?0:tx2>=color.width()?color.width() - 1:tx2,
                                                   ty2<=0?0:ty2>=color.height()?color.height() - 1:ty2)._data,opacity).
              draw_point(x3,y3,color.get_vector_at(tx3<=0?0:tx3>=color.width()?color.width() - 1:tx3,
                                                   ty3<=0?0:ty3>=color.height()?color.height() - 1:ty3)._data,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawDot((float)x0,height() - (float)y0);
              board.drawDot((float)x1,height() - (float)y1);
              board.drawDot((float)x2,height() - (float)y2);
              board.drawDot((float)x3,height() - (float)y3);
            }
#endif
            break;
          case 1 :
            if (zbuffer)
              draw_line(zbuffer,x0,y0,z0,x1,y1,z1,color,tx0,ty0,tx1,ty1,opacity).
                draw_line(zbuffer,x1,y1,z1,x2,y2,z2,color,tx1,ty1,tx2,ty2,opacity).
                draw_line(zbuffer,x2,y2,z2,x3,y3,z3,color,tx2,ty2,tx3,ty3,opacity).
                draw_line(zbuffer,x3,y3,z3,x0,y0,z0,color,tx3,ty3,tx0,ty0,opacity);
            else
              draw_line(x0,y0,z0,x1,y1,z1,color,tx0,ty0,tx1,ty1,opacity).
                draw_line(x1,y1,z1,x2,y2,z2,color,tx1,ty1,tx2,ty2,opacity).
                draw_line(x2,y2,z2,x3,y3,z3,color,tx2,ty2,tx3,ty3,opacity).
                draw_line(x3,y3,z3,x0,y0,z0,color,tx3,ty3,tx0,ty0,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.drawLine((float)x0,height() - (float)y0,(float)x1,height() - (float)y1);
              board.drawLine((float)x1,height() - (float)y1,(float)x2,height() - (float)y2);
              board.drawLine((float)x2,height() - (float)y2,(float)x3,height() - (float)y3);
              board.drawLine((float)x3,height() - (float)y3,(float)x0,height() - (float)y0);
            }
#endif
            break;
          case 2 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,opacity);
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity).
                draw_triangle(x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x2,height() - (float)y2,
                                 (float)x3,height() - (float)y3);
            }
#endif
            break;
          case 3 :
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l)).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,opacity,lightprops(l));
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,opacity,lightprops(l)).
                draw_triangle(x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,opacity,lightprops(l));
#ifdef cimg_use_board
            if (pboard) {
              const float lp = std::min(lightprops(l),1);
              board.setPenColorRGBi((unsigned char)(128*lp),
                                     (unsigned char)(128*lp),
                                     (unsigned char)(128*lp),
                                     (unsigned char)(opacity*255));
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x1,height() - (float)y1,
                                 (float)x2,height() - (float)y2);
              board.fillTriangle((float)x0,height() - (float)y0,
                                 (float)x2,height() - (float)y2,
                                 (float)x3,height() - (float)y3);
            }
#endif
            break;
          case 4 : {
            const float
              lightprop0 = lightprops(n0), lightprop1 = lightprops(n1),
              lightprop2 = lightprops(n2), lightprop3 = lightprops(n3);
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            lightprop0,lightprop1,lightprop2,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,
                              lightprop0,lightprop2,lightprop3,opacity);
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            lightprop0,lightprop1,lightprop2,opacity).
                draw_triangle(x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,
                              lightprop0,lightprop2,lightprop3,opacity);
#ifdef cimg_use_board
            if (pboard) {
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,lightprop0,
                                         (float)x1,height() - (float)y1,lightprop1,
                                         (float)x2,height() - (float)y2,lightprop2);
              board.fillGouraudTriangle((float)x0,height()  -(float)y0,lightprop0,
                                         (float)x2,height() - (float)y2,lightprop2,
                                         (float)x3,height() - (float)y3,lightprop3);
            }
#endif
          } break;
          case 5 : {
            const unsigned int
              lx0 = (unsigned int)lightprops(n0,0), ly0 = (unsigned int)lightprops(n0,1),
              lx1 = (unsigned int)lightprops(n1,0), ly1 = (unsigned int)lightprops(n1,1),
              lx2 = (unsigned int)lightprops(n2,0), ly2 = (unsigned int)lightprops(n2,1),
              lx3 = (unsigned int)lightprops(n3,0), ly3 = (unsigned int)lightprops(n3,1);
            if (zbuffer)
              draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity).
                draw_triangle(zbuffer,x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,
                              light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity);
            else
              draw_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,color,tx0,ty0,tx1,ty1,tx2,ty2,
                            light_texture,lx0,ly0,lx1,ly1,lx2,ly2,opacity).
                draw_triangle(x0,y0,z0,x2,y2,z2,x3,y3,z3,color,tx0,ty0,tx2,ty2,tx3,ty3,
                              light_texture,lx0,ly0,lx2,ly2,lx3,ly3,opacity);
#ifdef cimg_use_board
            if (pboard) {
              const float
                l0 = light_texture((int)(light_texture.width()/2*(1 + lx0)), (int)(light_texture.height()/2*(1 + ly0))),
                l1 = light_texture((int)(light_texture.width()/2*(1 + lx1)), (int)(light_texture.height()/2*(1 + ly1))),
                l2 = light_texture((int)(light_texture.width()/2*(1 + lx2)), (int)(light_texture.height()/2*(1 + ly2))),
                l3 = light_texture((int)(light_texture.width()/2*(1 + lx3)), (int)(light_texture.height()/2*(1 + ly3)));
              board.setPenColorRGBi(128,128,128,(unsigned char)(opacity*255));
              board.fillGouraudTriangle((float)x0,height() - (float)y0,l0,
                                         (float)x1,height() - (float)y1,l1,
                                         (float)x2,height() - (float)y2,l2);
              board.fillGouraudTriangle((float)x0,height()  -(float)y0,l0,
                                         (float)x2,height() - (float)y2,l2,
                                         (float)x3,height() - (float)y3,l3);
            }
#endif
          } break;
          }
        } break;
        }
      }

      if (render_type==5) cimg::mutex(10,0);
      return *this;