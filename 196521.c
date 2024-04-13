void LibRaw::phase_one_subtract_black(ushort *src, ushort *dest)
{
  //	ushort *src = (ushort*)imgdata.rawdata.raw_alloc;
  if(O.user_black<0 && O.user_cblack[0] <= -1000000 && O.user_cblack[1] <= -1000000 && O.user_cblack[2] <= -1000000 && O.user_cblack[3] <= -1000000)
    {
      for(int row = 0; row < S.raw_height; row++)
        {
          ushort bl = imgdata.color.phase_one_data.t_black - imgdata.rawdata.ph1_black[row][0];
          for(int col=0; col < imgdata.color.phase_one_data.split_col && col < S.raw_width; col++)
            {
              int idx  = row*S.raw_width + col;
              ushort val = src[idx];
              dest[idx] = val>bl?val-bl:0;
            }
          bl = imgdata.color.phase_one_data.t_black - imgdata.rawdata.ph1_black[row][1];
          for(int col=imgdata.color.phase_one_data.split_col; col < S.raw_width; col++)
            {
              int idx  = row*S.raw_width + col;
              ushort val = src[idx];
              dest[idx] = val>bl?val-bl:0;
            }
        }
    }
  else // black set by user interaction
    {
      // Black level in cblack!
      for(int row = 0; row < S.raw_height; row++)
        {
          unsigned short cblk[16];
          for(int cc=0; cc<16;cc++)
            cblk[cc]=C.cblack[fcol(row,cc)];
          for(int col = 0; col < S.raw_width; col++)
            {
              int idx  = row*S.raw_width + col;
              ushort val = src[idx];
              ushort bl = cblk[col&0xf];
              dest[idx] = val>bl?val-bl:0;
            }
        }
    }
}