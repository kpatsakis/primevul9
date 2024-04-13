void CLASS canon_rmf_load_raw()
{
  int row, col, bits, orow, ocol, c;

  for (row=0; row < raw_height; row++)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    for (col=0; col < raw_width-2; col+=3) {
      bits = get4();
      FORC3 {
	orow = row;
	if ((ocol = col+c-4) < 0) {
	  ocol += raw_width;
	  if ((orow -= 2) < 0)
	    orow += raw_height;
	}
	RAW(orow,ocol) = curve[bits >> (10*c+2) & 0x3ff];
      }
    }
  }  
  maximum = curve[0x3ff];
}