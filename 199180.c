    //! Load image from a camera stream, using OpenCV \newinstance.
    static CImg<T> get_load_camera(const unsigned int camera_index=0, const unsigned int skip_frames=0,
                                   const bool release_camera=true,
                                   const unsigned int capture_width=0, const unsigned int capture_height=0) {
      return CImg<T>().load_camera(camera_index,skip_frames,release_camera,capture_width,capture_height);