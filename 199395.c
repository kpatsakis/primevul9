    //! Compute correspondence map between two images, using the patch-match algorithm \overloading.
    CImg<intT> get_patchmatch(const CImg<T>& patch_image,
                              const unsigned int patch_width,
                              const unsigned int patch_height,
                              const unsigned int patch_depth=1,
                              const unsigned int nb_iterations=5,
                              const unsigned int nb_randoms=5) const {
      return _patchmatch(patch_image,patch_width,patch_height,patch_depth,
                         nb_iterations,nb_randoms,
                         CImg<T>::const_empty(),
                         false,CImg<T>::empty());