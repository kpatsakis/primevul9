    template<typename t>
    CImg<Tfloat> get_blur_guided(const CImg<t>& guide, const float radius, const float regularization) const {
      if (!is_sameXYZ(guide))
        throw CImgArgumentException(_cimg_instance
                                    "blur_guided(): Invalid size for specified guide image (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    guide._width,guide._height,guide._depth,guide._spectrum,guide._data);
      if (is_empty() || !radius) return *this;
      const int _radius = radius>=0?(int)radius:(int)(-radius*cimg::max(_width,_height,_depth)/100);
      float _regularization = regularization;
      if (regularization<0) {
        T edge_min, edge_max = guide.max_min(edge_min);
        if (edge_min==edge_max) return *this;
        _regularization = -regularization*(edge_max - edge_min)/100;
      }
      _regularization = std::max(_regularization,0.01f);
      const unsigned int psize = (unsigned int)(1 + 2*_radius);
      const CImg<uintT> N = CImg<uintT>(_width,_height,_depth,1,1)._blur_guided(psize);
      CImg<Tfloat>
        mean_I = CImg<Tfloat>(guide,false)._blur_guided(psize).div(N),
        mean_p = CImg<Tfloat>(*this,false)._blur_guided(psize).div(N),
        cov_Ip = CImg<Tfloat>(*this,false).mul(guide)._blur_guided(psize).div(N)-=mean_p.get_mul(mean_I),
        var_I = CImg<Tfloat>(guide,false).sqr()._blur_guided(psize).div(N)-=mean_I.get_sqr(),
        &a = cov_Ip.div(var_I+=_regularization),
        &b = mean_p-=a.get_mul(mean_I);
      a._blur_guided(psize).div(N);
      b._blur_guided(psize).div(N);
      return a.mul(guide)+=b;