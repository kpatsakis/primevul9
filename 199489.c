    template<typename t>
    CImg<intT> get_patchmatch(const CImg<T>& patch_image,
                              const unsigned int patch_width,
                              const unsigned int patch_height,
                              const unsigned int patch_depth,
                              const unsigned int nb_iterations,
                              const unsigned int nb_randoms,
                              const CImg<t> &guide) const {
      return _patchmatch(patch_image,patch_width,patch_height,patch_depth,
                         nb_iterations,nb_randoms,
                         guide,false,CImg<T>::empty());