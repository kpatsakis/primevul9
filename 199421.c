    **/
    CImg<T>& load_camera(const unsigned int camera_index=0, const unsigned int skip_frames=0,
                         const bool release_camera=true, const unsigned int capture_width=0,
                         const unsigned int capture_height=0) {
#ifdef cimg_use_opencv
      if (camera_index>99)
        throw CImgArgumentException(_cimg_instance
                                    "load_camera(): Invalid request for camera #%u "
                                    "(no more than 100 cameras can be managed simultaneously).",
                                    cimg_instance,
                                    camera_index);
      static CvCapture *capture[100] = { 0 };
      static unsigned int capture_w[100], capture_h[100];
      if (release_camera) {
        cimg::mutex(9);
        if (capture[camera_index]) cvReleaseCapture(&(capture[camera_index]));
        capture[camera_index] = 0;
        capture_w[camera_index] = capture_h[camera_index] = 0;
        cimg::mutex(9,0);
        return *this;
      }
      if (!capture[camera_index]) {
        cimg::mutex(9);
        capture[camera_index] = cvCreateCameraCapture(camera_index);
        capture_w[camera_index] = 0;
        capture_h[camera_index] = 0;
        cimg::mutex(9,0);
        if (!capture[camera_index]) {
          throw CImgIOException(_cimg_instance
                                "load_camera(): Failed to initialize camera #%u.",
                                cimg_instance,
                                camera_index);
        }
      }
      cimg::mutex(9);
      if (capture_width!=capture_w[camera_index]) {
        cvSetCaptureProperty(capture[camera_index],CV_CAP_PROP_FRAME_WIDTH,capture_width);
        capture_w[camera_index] = capture_width;
      }
      if (capture_height!=capture_h[camera_index]) {
        cvSetCaptureProperty(capture[camera_index],CV_CAP_PROP_FRAME_HEIGHT,capture_height);
        capture_h[camera_index] = capture_height;
      }
      const IplImage *img = 0;
      for (unsigned int i = 0; i<skip_frames; ++i) img = cvQueryFrame(capture[camera_index]);
      img = cvQueryFrame(capture[camera_index]);
      if (img) {
        const int step = (int)(img->widthStep - 3*img->width);
        assign(img->width,img->height,1,3);
        const unsigned char* ptrs = (unsigned char*)img->imageData;
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        if (step>0) cimg_forY(*this,y) {
            cimg_forX(*this,x) { *(ptr_b++) = (T)*(ptrs++); *(ptr_g++) = (T)*(ptrs++); *(ptr_r++) = (T)*(ptrs++); }
            ptrs+=step;
          } else for (ulongT siz = (ulongT)img->width*img->height; siz; --siz) {
            *(ptr_b++) = (T)*(ptrs++); *(ptr_g++) = (T)*(ptrs++); *(ptr_r++) = (T)*(ptrs++);
          }
      }
      cimg::mutex(9,0);
      return *this;
#else
      cimg::unused(camera_index,skip_frames,release_camera,capture_width,capture_height);
      throw CImgIOException(_cimg_instance
                            "load_camera(): This function requires the OpenCV library to run "
                            "(macro 'cimg_use_opencv' must be defined).",
                            cimg_instance);
#endif