    //! Blur image using patch-based space \newinstance.
    CImg<Tfloat> get_blur_patch(const float sigma_s, const float sigma_p, const unsigned int patch_size=3,
                                const unsigned int lookup_size=4, const float smoothness=0,
                                const bool is_fast_approx=true) const {

#define _cimg_blur_patch3d_fast(N) \
      cimg_for##N##XYZ(res,x,y,z) { \
        T *pP = P._data; cimg_forC(res,c) { cimg_get##N##x##N##x##N(img,x,y,z,c,pP,T); pP+=N3; } \
        const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1, \
          x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2; \
        float sum_weights = 0; \
        cimg_for_in##N##XYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) \
          if (cimg::abs((Tfloat)img(x,y,z,0) - (Tfloat)img(p,q,r,0))<sigma_p3) { \
            T *pQ = Q._data; cimg_forC(res,c) { cimg_get##N##x##N##x##N(img,p,q,r,c,pQ,T); pQ+=N3; } \
            float distance2 = 0; \
            pQ = Q._data; cimg_for(P,pP,T) { const float dI = (float)*pP - (float)*(pQ++); distance2+=dI*dI; } \
            distance2/=Pnorm; \
            const float dx = (float)p - x, dy = (float)q - y, dz = (float)r - z, \
              alldist = distance2 + (dx*dx + dy*dy + dz*dz)/sigma_s2, weight = alldist>3?0.0f:1.0f; \
            sum_weights+=weight; \
            cimg_forC(res,c) res(x,y,z,c)+=weight*(*this)(p,q,r,c); \
          } \
        if (sum_weights>0) cimg_forC(res,c) res(x,y,z,c)/=sum_weights; \
        else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c)); \
    }

#define _cimg_blur_patch3d(N) \
      cimg_for##N##XYZ(res,x,y,z) { \
        T *pP = P._data; cimg_forC(res,c) { cimg_get##N##x##N##x##N(img,x,y,z,c,pP,T); pP+=N3; } \
        const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1, \
          x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2; \
        float sum_weights = 0, weight_max = 0; \
        cimg_for_in##N##XYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) if (p!=x || q!=y || r!=z) { \
          T *pQ = Q._data; cimg_forC(res,c) { cimg_get##N##x##N##x##N(img,p,q,r,c,pQ,T); pQ+=N3; } \
          float distance2 = 0; \
          pQ = Q._data; cimg_for(P,pP,T) { const float dI = (float)*pP - (float)*(pQ++); distance2+=dI*dI; } \
          distance2/=Pnorm; \
          const float dx = (float)p - x, dy = (float)q - y, dz = (float)r - z, \
            alldist = distance2 + (dx*dx + dy*dy + dz*dz)/sigma_s2, weight = (float)std::exp(-alldist); \
          if (weight>weight_max) weight_max = weight; \
          sum_weights+=weight; \
          cimg_forC(res,c) res(x,y,z,c)+=weight*(*this)(p,q,r,c); \
        } \
        sum_weights+=weight_max; cimg_forC(res,c) res(x,y,z,c)+=weight_max*(*this)(x,y,z,c); \
        if (sum_weights>0) cimg_forC(res,c) res(x,y,z,c)/=sum_weights; \
        else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c)); \
      }

#define _cimg_blur_patch2d_fast(N) \
        cimg_for##N##XY(res,x,y) { \
          T *pP = P._data; cimg_forC(res,c) { cimg_get##N##x##N(img,x,y,0,c,pP,T); pP+=N2; } \
          const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2; \
          float sum_weights = 0; \
          cimg_for_in##N##XY(res,x0,y0,x1,y1,p,q) \
            if (cimg::abs((Tfloat)img(x,y,0,0) - (Tfloat)img(p,q,0,0))<sigma_p3) { \
              T *pQ = Q._data; cimg_forC(res,c) { cimg_get##N##x##N(img,p,q,0,c,pQ,T); pQ+=N2; } \
              float distance2 = 0; \
              pQ = Q._data; cimg_for(P,pP,T) { const float dI = (float)*pP - (float)*(pQ++); distance2+=dI*dI; } \
              distance2/=Pnorm; \
              const float dx = (float)p - x, dy = (float)q - y, \
                alldist = distance2 + (dx*dx+dy*dy)/sigma_s2, weight = alldist>3?0.0f:1.0f; \
              sum_weights+=weight; \
              cimg_forC(res,c) res(x,y,c)+=weight*(*this)(p,q,c); \
            } \
          if (sum_weights>0) cimg_forC(res,c) res(x,y,c)/=sum_weights; \
          else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c)); \
        }

#define _cimg_blur_patch2d(N) \
        cimg_for##N##XY(res,x,y) { \
          T *pP = P._data; cimg_forC(res,c) { cimg_get##N##x##N(img,x,y,0,c,pP,T); pP+=N2; } \
          const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2; \
          float sum_weights = 0, weight_max = 0; \
          cimg_for_in##N##XY(res,x0,y0,x1,y1,p,q) if (p!=x || q!=y) { \
            T *pQ = Q._data; cimg_forC(res,c) { cimg_get##N##x##N(img,p,q,0,c,pQ,T); pQ+=N2; } \
            float distance2 = 0; \
            pQ = Q._data; cimg_for(P,pP,T) { const float dI = (float)*pP - (float)*(pQ++); distance2+=dI*dI; } \
            distance2/=Pnorm; \
            const float dx = (float)p - x, dy = (float)q - y, \
              alldist = distance2 + (dx*dx+dy*dy)/sigma_s2, weight = (float)std::exp(-alldist); \
            if (weight>weight_max) weight_max = weight; \
            sum_weights+=weight; \
            cimg_forC(res,c) res(x,y,c)+=weight*(*this)(p,q,c); \
          } \
          sum_weights+=weight_max; cimg_forC(res,c) res(x,y,c)+=weight_max*(*this)(x,y,c); \
          if (sum_weights>0) cimg_forC(res,c) res(x,y,c)/=sum_weights; \
          else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c)); \
    }

      if (is_empty() || !patch_size || !lookup_size) return +*this;
      CImg<Tfloat> res(_width,_height,_depth,_spectrum,0);
      const CImg<T> _img = smoothness>0?get_blur(smoothness):CImg<Tfloat>(),&img = smoothness>0?_img:*this;
      CImg<T> P(patch_size*patch_size*_spectrum), Q(P);
      const float
        nsigma_s = sigma_s>=0?sigma_s:-sigma_s*cimg::max(_width,_height,_depth)/100,
        sigma_s2 = nsigma_s*nsigma_s, sigma_p2 = sigma_p*sigma_p, sigma_p3 = 3*sigma_p,
        Pnorm = P.size()*sigma_p2;
      const int rsize2 = (int)lookup_size/2, rsize1 = (int)lookup_size - rsize2 - 1;
      const unsigned int N2 = patch_size*patch_size, N3 = N2*patch_size;
      cimg::unused(N2,N3);
      if (_depth>1) switch (patch_size) { // 3d
        case 2 : if (is_fast_approx) _cimg_blur_patch3d_fast(2) else _cimg_blur_patch3d(2) break;
        case 3 : if (is_fast_approx) _cimg_blur_patch3d_fast(3) else _cimg_blur_patch3d(3) break;
        default : {
          const int psize2 = (int)patch_size/2, psize1 = (int)patch_size - psize2 - 1;
          if (is_fast_approx)
            cimg_pragma_openmp(parallel for collapse(2) cimg_openmp_if(res._width>=32 && res._height*res._depth>=4)
                               private(P,Q))
            cimg_forXYZ(res,x,y,z) { // Fast
              P = img.get_crop(x - psize1,y - psize1,z - psize1,x + psize2,y + psize2,z + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1,
                x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2;
              float sum_weights = 0;
              cimg_for_inXYZ(res,x0,y0,z0,x1,y1,z1,p,q,r)
                if (cimg::abs((Tfloat)img(x,y,z,0) - (Tfloat)img(p,q,r,0))<sigma_p3) {
                  (Q = img.get_crop(p - psize1,q - psize1,r - psize1,p + psize2,q + psize2,r + psize2,true))-=P;
                  const float
                    dx = (float)x - p, dy = (float)y - q, dz = (float)z - r,
                    distance2 = (float)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy + dz*dz)/sigma_s2),
                    weight = distance2>3?0.0f:1.0f;
                  sum_weights+=weight;
                  cimg_forC(res,c) res(x,y,z,c)+=weight*(*this)(p,q,r,c);
                }
              if (sum_weights>0) cimg_forC(res,c) res(x,y,z,c)/=sum_weights;
              else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c));
            } else
            cimg_pragma_openmp(parallel for collapse(2)
                               if (res._width>=32 && res._height*res._depth>=4) firstprivate(P,Q))
            cimg_forXYZ(res,x,y,z) { // Exact
              P = img.get_crop(x - psize1,y - psize1,z - psize1,x + psize2,y + psize2,z + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, z0 = z - rsize1,
                x1 = x + rsize2, y1 = y + rsize2, z1 = z + rsize2;
              float sum_weights = 0, weight_max = 0;
              cimg_for_inXYZ(res,x0,y0,z0,x1,y1,z1,p,q,r) if (p!=x || q!=y || r!=z) {
                (Q = img.get_crop(p - psize1,q - psize1,r - psize1,p + psize2,q + psize2,r + psize2,true))-=P;
                const float
                  dx = (float)x - p, dy = (float)y - q, dz = (float)z - r,
                  distance2 = (float)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy + dz*dz)/sigma_s2),
                  weight = (float)std::exp(-distance2);
                if (weight>weight_max) weight_max = weight;
                sum_weights+=weight;
                cimg_forC(res,c) res(x,y,z,c)+=weight*(*this)(p,q,r,c);
              }
              sum_weights+=weight_max; cimg_forC(res,c) res(x,y,z,c)+=weight_max*(*this)(x,y,z,c);
              if (sum_weights>0) cimg_forC(res,c) res(x,y,z,c)/=sum_weights;
              else cimg_forC(res,c) res(x,y,z,c) = (Tfloat)((*this)(x,y,z,c));
            }
        }
        } else switch (patch_size) { // 2d
        case 2 : if (is_fast_approx) _cimg_blur_patch2d_fast(2) else _cimg_blur_patch2d(2) break;
        case 3 : if (is_fast_approx) _cimg_blur_patch2d_fast(3) else _cimg_blur_patch2d(3) break;
        case 4 : if (is_fast_approx) _cimg_blur_patch2d_fast(4) else _cimg_blur_patch2d(4) break;
        case 5 : if (is_fast_approx) _cimg_blur_patch2d_fast(5) else _cimg_blur_patch2d(5) break;
        case 6 : if (is_fast_approx) _cimg_blur_patch2d_fast(6) else _cimg_blur_patch2d(6) break;
        case 7 : if (is_fast_approx) _cimg_blur_patch2d_fast(7) else _cimg_blur_patch2d(7) break;
        case 8 : if (is_fast_approx) _cimg_blur_patch2d_fast(8) else _cimg_blur_patch2d(8) break;
        case 9 : if (is_fast_approx) _cimg_blur_patch2d_fast(9) else _cimg_blur_patch2d(9) break;
        default : { // Fast
          const int psize2 = (int)patch_size/2, psize1 = (int)patch_size - psize2 - 1;
          if (is_fast_approx)
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=32 && res._height>=4) firstprivate(P,Q))
            cimg_forXY(res,x,y) { // 2d fast approximation.
              P = img.get_crop(x - psize1,y - psize1,x + psize2,y + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2;
              float sum_weights = 0;
              cimg_for_inXY(res,x0,y0,x1,y1,p,q)
                if ((Tfloat)cimg::abs(img(x,y,0) - (Tfloat)img(p,q,0))<sigma_p3) {
                  (Q = img.get_crop(p - psize1,q - psize1,p + psize2,q + psize2,true))-=P;
                  const float
                    dx = (float)x - p, dy = (float)y - q,
                    distance2 = (float)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy)/sigma_s2),
                    weight = distance2>3?0.0f:1.0f;
                  sum_weights+=weight;
                  cimg_forC(res,c) res(x,y,c)+=weight*(*this)(p,q,c);
                }
              if (sum_weights>0) cimg_forC(res,c) res(x,y,c)/=sum_weights;
              else cimg_forC(res,c) res(x,y,c) = (Tfloat)((*this)(x,y,c));
            } else
            cimg_pragma_openmp(parallel for cimg_openmp_if(res._width>=32 && res._height>=4) firstprivate(P,Q))
            cimg_forXY(res,x,y) { // 2d exact algorithm.
              P = img.get_crop(x - psize1,y - psize1,x + psize2,y + psize2,true);
              const int x0 = x - rsize1, y0 = y - rsize1, x1 = x + rsize2, y1 = y + rsize2;
              float sum_weights = 0, weight_max = 0;
              cimg_for_inXY(res,x0,y0,x1,y1,p,q) if (p!=x || q!=y) {
                (Q = img.get_crop(p - psize1,q - psize1,p + psize2,q + psize2,true))-=P;
                const float
                  dx = (float)x - p, dy = (float)y - q,
                  distance2 = (float)(Q.pow(2).sum()/Pnorm + (dx*dx + dy*dy)/sigma_s2),
                  weight = (float)std::exp(-distance2);
                if (weight>weight_max) weight_max = weight;
                sum_weights+=weight;
                cimg_forC(res,c) res(x,y,c)+=weight*(*this)(p,q,c);
              }
              sum_weights+=weight_max; cimg_forC(res,c) res(x,y,c)+=weight_max*(*this)(x,y,c);
              if (sum_weights>0) cimg_forC(res,c) res(x,y,c)/=sum_weights;
              else cimg_forC(res,c) res(x,y,0,c) = (Tfloat)((*this)(x,y,c));
            }
        }
        }
      return res;