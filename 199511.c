    template<typename t1, typename t2>
    CImg<T>& draw_quiver(const CImg<t1>& flow,
                         const CImg<t2>& color, const float opacity=1,
                         const unsigned int sampling=25, const float factor=-20,
                         const bool is_arrow=true, const unsigned int pattern=~0U) {
      if (is_empty()) return *this;
      if (!flow || flow._spectrum!=2)
        throw CImgArgumentException(_cimg_instance
                                    "draw_quiver(): Invalid dimensions of specified flow (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    flow._width,flow._height,flow._depth,flow._spectrum,flow._data);
      if (sampling<=0)
        throw CImgArgumentException(_cimg_instance
                                    "draw_quiver(): Invalid sampling value %g "
                                    "(should be >0)",
                                    cimg_instance,
                                    sampling);
      const bool colorfield = (color._width==flow._width && color._height==flow._height &&
                               color._depth==1 && color._spectrum==_spectrum);
      if (is_overlapped(flow)) return draw_quiver(+flow,color,opacity,sampling,factor,is_arrow,pattern);
      float vmax,fact;
      if (factor<=0) {
        float m, M = (float)flow.get_norm(2).max_min(m);
        vmax = (float)std::max(cimg::abs(m),cimg::abs(M));
        if (!vmax) vmax = 1;
        fact = -factor;
      } else { fact = factor; vmax = 1; }

      for (unsigned int y = sampling/2; y<_height; y+=sampling)
        for (unsigned int x = sampling/2; x<_width; x+=sampling) {
          const unsigned int X = x*flow._width/_width, Y = y*flow._height/_height;
          float u = (float)flow(X,Y,0,0)*fact/vmax, v = (float)flow(X,Y,0,1)*fact/vmax;
          if (is_arrow) {
            const int xx = (int)(x + u), yy = (int)(y + v);
            if (colorfield) draw_arrow(x,y,xx,yy,color.get_vector_at(X,Y)._data,opacity,45,sampling/5.0f,pattern);
            else draw_arrow(x,y,xx,yy,color._data,opacity,45,sampling/5.0f,pattern);
          } else {
            if (colorfield)
              draw_line((int)(x - 0.5*u),(int)(y - 0.5*v),(int)(x + 0.5*u),(int)(y + 0.5*v),
                        color.get_vector_at(X,Y)._data,opacity,pattern);
            else draw_line((int)(x - 0.5*u),(int)(y - 0.5*v),(int)(x + 0.5*u),(int)(y + 0.5*v),
                           color._data,opacity,pattern);
          }
        }
      return *this;