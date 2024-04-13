       \param codec Type of compression (See http://www.fourcc.org/codecs.php to see available codecs).
       \param keep_open Tells if the video writer associated to the specified filename
       must be kept open or not (to allow frames to be added in the same file afterwards).
    **/
    const CImgList<T>& save_video(const char *const filename, const unsigned int fps=25,
                                  const char *codec=0, const bool keep_open=false) const {
#ifndef cimg_use_opencv
      cimg::unused(codec,keep_open);
      return save_ffmpeg_external(filename,fps);
#else
      static CvVideoWriter *writers[32] = { 0 };
      static CImgList<charT> filenames(32);
      static CImg<intT> sizes(32,2,1,1,0);
      static int last_used_index = -1;

      // Detect if a video writer already exists for the specified filename.
      cimg::mutex(9);
      int index = -1;
      if (filename) {
        if (last_used_index>=0 && !std::strcmp(filename,filenames[last_used_index])) {
          index = last_used_index;
        } else cimglist_for(filenames,l) if (filenames[l] && !std::strcmp(filename,filenames[l])) {
            index = l; break;
          }
      } else index = last_used_index;
      cimg::mutex(9,0);

      // Find empty slot for capturing video stream.
      if (index<0) {
        if (!filename)
          throw CImgArgumentException(_cimglist_instance
                                      "save_video(): No already open video writer found. You must specify a "
                                      "non-(null) filename argument for the first call.",
                                      cimglist_instance);
        else { cimg::mutex(9); cimglist_for(filenames,l) if (!filenames[l]) { index = l; break; } cimg::mutex(9,0); }
        if (index<0)
          throw CImgIOException(_cimglist_instance
                                "save_video(): File '%s', no video writer slots available. "
                                "You have to release some of your previously opened videos.",
                                cimglist_instance,filename);
        if (is_empty())
          throw CImgInstanceException(_cimglist_instance
                                      "save_video(): Instance list is empty.",
                                      cimglist_instance);
        const unsigned int W = _data?_data[0]._width:0, H = _data?_data[0]._height:0;
        if (!W || !H)
          throw CImgInstanceException(_cimglist_instance
                                      "save_video(): Frame [0] is an empty image.",
                                      cimglist_instance);

#define _cimg_docase(x) ((x)>='a'&&(x)<='z'?(x) + 'A' - 'a':(x))

        const char
          *const _codec = codec && *codec?codec:cimg_OS==2?"mpeg":"mp4v",
          codec0 = _cimg_docase(_codec[0]),
          codec1 = _codec[0]?_cimg_docase(_codec[1]):0,
          codec2 = _codec[1]?_cimg_docase(_codec[2]):0,
          codec3 = _codec[2]?_cimg_docase(_codec[3]):0;
        cimg::mutex(9);
        writers[index] = cvCreateVideoWriter(filename,CV_FOURCC(codec0,codec1,codec2,codec3),
                                             fps,cvSize(W,H));
        CImg<charT>::string(filename).move_to(filenames[index]);
        sizes(index,0) = W; sizes(index,1) = H;
        cimg::mutex(9,0);
        if (!writers[index])
          throw CImgIOException(_cimglist_instance
                                "save_video(): File '%s', unable to initialize video writer with codec '%c%c%c%c'.",
                                cimglist_instance,filename,
                                codec0,codec1,codec2,codec3);
      }

      if (!is_empty()) {
        const unsigned int W = sizes(index,0), H = sizes(index,1);
        cimg::mutex(9);
        IplImage *ipl = cvCreateImage(cvSize(W,H),8,3);
        cimglist_for(*this,l) {
          CImg<T> &src = _data[l];
          if (src.is_empty())
            cimg::warn(_cimglist_instance
                       "save_video(): Skip empty frame %d for file '%s'.",
                       cimglist_instance,l,filename);
          if (src._depth>1 || src._spectrum>3)
            cimg::warn(_cimglist_instance
                       "save_video(): Frame %u has incompatible dimension (%u,%u,%u,%u). "
                       "Some image data may be ignored when writing frame into video file '%s'.",
                       cimglist_instance,l,src._width,src._height,src._depth,src._spectrum,filename);
          if (src._width==W && src._height==H && src._spectrum==3) {
            const T *ptr_r = src.data(0,0,0,0), *ptr_g = src.data(0,0,0,1), *ptr_b = src.data(0,0,0,2);
            char *ptrd = ipl->imageData;
            cimg_forXY(src,x,y) {
              *(ptrd++) = (char)*(ptr_b++); *(ptrd++) = (char)*(ptr_g++); *(ptrd++) = (char)*(ptr_r++);
            }
          } else {
            CImg<unsigned char> _src(src,false);
            _src.channels(0,std::min(_src._spectrum - 1,2U)).resize(W,H);
            _src.resize(W,H,1,3,_src._spectrum==1);
            const unsigned char *ptr_r = _src.data(0,0,0,0), *ptr_g = _src.data(0,0,0,1), *ptr_b = _src.data(0,0,0,2);
            char *ptrd = ipl->imageData;
            cimg_forXY(_src,x,y) {
              *(ptrd++) = (char)*(ptr_b++); *(ptrd++) = (char)*(ptr_g++); *(ptrd++) = (char)*(ptr_r++);
            }
          }
          cvWriteFrame(writers[index],ipl);
        }
        cvReleaseImage(&ipl);
        cimg::mutex(9,0);
      }

      cimg::mutex(9);
      if (!keep_open) {
        cvReleaseVideoWriter(&writers[index]);
        writers[index] = 0;
        filenames[index].assign();
        sizes(index,0) = sizes(index,1) = 0;
        last_used_index = -1;
      } else last_used_index = index;
      cimg::mutex(9,0);