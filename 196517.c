int LibRaw::copy_mem_image(void* scan0, int stride, int bgr)

{
    // the image memory pointed to by scan0 is assumed to be in the format returned by get_mem_image_format
    if((imgdata.progress_flags & LIBRAW_PROGRESS_THUMB_MASK) < LIBRAW_PROGRESS_PRE_INTERPOLATE)
        return LIBRAW_OUT_OF_ORDER_CALL;

    if(libraw_internal_data.output_data.histogram)
      {
        int perc, val, total, t_white=0x2000,c;
        perc = S.width * S.height * 0.01;        /* 99th percentile white level */
        if (IO.fuji_width) perc /= 2;
        if (!((O.highlight & ~2) || O.no_auto_bright))
          for (t_white=c=0; c < P1.colors; c++) {
            for (val=0x2000, total=0; --val > 32; )
              if ((total += libraw_internal_data.output_data.histogram[c][val]) > perc) break;
            if (t_white < val) t_white = val;
          }
        gamma_curve (O.gamm[0], O.gamm[1], 2, (t_white << 3)/O.bright);
      }
    
    int s_iheight = S.iheight;
    int s_iwidth = S.iwidth;
    int s_width = S.width;
    int s_hwight = S.height;

    S.iheight = S.height;
    S.iwidth  = S.width;

    if (S.flip & 4) SWAP(S.height,S.width);
    uchar *ppm;
    ushort *ppm2;
    int c, row, col, soff, rstep, cstep;

    soff  = flip_index (0, 0);
    cstep = flip_index (0, 1) - soff;
    rstep = flip_index (1, 0) - flip_index (0, S.width);

    for (row=0; row < S.height; row++, soff += rstep) 
      {
        uchar *bufp = ((uchar*)scan0)+row*stride;
        ppm2 = (ushort*) (ppm = bufp);
        // keep trivial decisions in the outer loop for speed
        if (bgr) {
          if (O.output_bps == 8) {
            for (col=0; col < S.width; col++, soff += cstep) 
              FORBGR *ppm++ = imgdata.color.curve[imgdata.image[soff][c]]>>8;
          }
          else {
            for (col=0; col < S.width; col++, soff += cstep) 
              FORBGR *ppm2++ = imgdata.color.curve[imgdata.image[soff][c]];
          }
        }
        else {
          if (O.output_bps == 8) {
            for (col=0; col < S.width; col++, soff += cstep) 
              FORRGB *ppm++ = imgdata.color.curve[imgdata.image[soff][c]]>>8;
          }
          else {
            for (col=0; col < S.width; col++, soff += cstep) 
              FORRGB *ppm2++ = imgdata.color.curve[imgdata.image[soff][c]];
          }
        }
        
//            bufp += stride;           // go to the next line
      }
 
    S.iheight = s_iheight;
    S.iwidth = s_iwidth;
    S.width = s_width;
    S.height = s_hwight;

    return 0;


}