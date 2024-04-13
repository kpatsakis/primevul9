void CLASS crop_masked_pixels()
{
  int row, col;
  unsigned 
#ifndef LIBRAW_LIBRARY_BUILD
    r, raw_pitch = raw_width*2,
    c, m, mblack[8], zero, val;
#else
    c, m, zero, val;
#define mblack imgdata.color.black_stat
#endif

#ifndef LIBRAW_LIBRARY_BUILD
  if (load_raw == &CLASS phase_one_load_raw ||
      load_raw == &CLASS phase_one_load_raw_c)
    phase_one_correct();
  if (fuji_width) {
    for (row=0; row < raw_height-top_margin*2; row++) {
      for (col=0; col < fuji_width << !fuji_layout; col++) {
	if (fuji_layout) {
	  r = fuji_width - 1 - col + (row >> 1);
	  c = col + ((row+1) >> 1);
	} else {
	  r = fuji_width - 1 + row - (col >> 1);
	  c = row + ((col+1) >> 1);
	}
	if (r < height && c < width)
	  BAYER(r,c) = RAW(row+top_margin,col+left_margin);
      }
    }
  } else {
    for (row=0; row < height; row++)
      for (col=0; col < width; col++)
	BAYER2(row,col) = RAW(row+top_margin,col+left_margin);
  }
#endif
  if (mask[0][3] > 0) goto mask_set;
  if (load_raw == &CLASS canon_load_raw ||
      load_raw == &CLASS lossless_jpeg_load_raw) {
    mask[0][1] = mask[1][1] += 2;
    mask[0][3] -= 2;
    goto sides;
  }
  if (load_raw == &CLASS canon_600_load_raw ||
      load_raw == &CLASS sony_load_raw ||
     (load_raw == &CLASS eight_bit_load_raw && strncmp(model,"DC2",3)) ||
      load_raw == &CLASS kodak_262_load_raw ||
     (load_raw == &CLASS packed_load_raw && (load_flags & 32))) {
sides:
    mask[0][0] = mask[1][0] = top_margin;
    mask[0][2] = mask[1][2] = top_margin+height;
    mask[0][3] += left_margin;
    mask[1][1] += left_margin+width;
    mask[1][3] += raw_width;
  }
  if (load_raw == &CLASS nokia_load_raw) {
    mask[0][2] = top_margin;
    mask[0][3] = width;
  }
mask_set:
  memset (mblack, 0, sizeof mblack);
  for (zero=m=0; m < 8; m++)
    for (row=MAX(mask[m][0],0); row < MIN(mask[m][2],raw_height); row++)
      for (col=MAX(mask[m][1],0); col < MIN(mask[m][3],raw_width); col++) {
	c = FC(row-top_margin,col-left_margin);
	mblack[c] += val = raw_image[(row)*raw_pitch/2+(col)];
	mblack[4+c]++;
	zero += !val;
      }
  if (load_raw == &CLASS canon_600_load_raw && width < raw_width) {
    black = (mblack[0]+mblack[1]+mblack[2]+mblack[3]) /
	    (mblack[4]+mblack[5]+mblack[6]+mblack[7]) - 4;
#ifndef LIBRAW_LIBRARY_BUILD
    canon_600_correct();
#endif
  } else if (zero < mblack[4] && mblack[5] && mblack[6] && mblack[7])
    FORC4 cblack[c] = mblack[c] / mblack[4+c];
}