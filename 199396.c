     **/
    CImg<T>& blur_anisotropic(const float amplitude, const float sharpness=0.7f, const float anisotropy=0.6f,
                              const float alpha=0.6f, const float sigma=1.1f, const float dl=0.8f, const float da=30,
                              const float gauss_prec=2, const unsigned int interpolation_type=0,
                              const bool is_fast_approx=true) {
      return blur_anisotropic(get_diffusion_tensors(sharpness,anisotropy,alpha,sigma,interpolation_type!=3),
                              amplitude,dl,da,gauss_prec,interpolation_type,is_fast_approx);