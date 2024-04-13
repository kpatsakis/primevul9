void LibRaw::scale_colors_loop(float scale_mul[4])
{
  unsigned size = S.iheight*S.iwidth;
  

  if (C.cblack[4] && C.cblack[5])
    {
      int val;
      for (unsigned i=0; i < size*4; i++) 
        {
          if (!(val = imgdata.image[0][i])) continue;
          val -= C.cblack[6 + i/4 / S.iwidth % C.cblack[4] * C.cblack[4] +
			i/4 % S.iwidth % C.cblack[5]];
          val -= C.cblack[i & 3];
          val *= scale_mul[i & 3];
          imgdata.image[0][i] = CLIP(val);
        }
    }
  else if(C.cblack[0]||C.cblack[1]||C.cblack[2]||C.cblack[3])
    {
      for (unsigned i=0; i < size*4; i++) 
        {
          int val = imgdata.image[0][i];
          if (!val) continue;
          val -= C.cblack[i & 3];
          val *= scale_mul[i & 3];
          imgdata.image[0][i] = CLIP(val);
        }
    }
  else // BL is zero
    {
      for (unsigned i=0; i < size*4; i++) 
        {
          int val = imgdata.image[0][i];
          val *= scale_mul[i & 3];
          imgdata.image[0][i] = CLIP(val);
        }
    }
}