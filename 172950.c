DLLEXPORT unsigned long DLLCALL tjBufSizeYUV2(int width, int pad, int height,
	int subsamp)
{
	int retval=0, nc, i;

	if(subsamp<0 || subsamp>=NUMSUBOPT)
		_throwg("tjBufSizeYUV2(): Invalid argument");

	nc=(subsamp==TJSAMP_GRAY? 1:3);
	for(i=0; i<nc; i++)
	{
		int pw=tjPlaneWidth(i, width, subsamp);
		int stride=PAD(pw, pad);
		int ph=tjPlaneHeight(i, height, subsamp);
		if(pw<0 || ph<0) return -1;
		else retval+=stride*ph;
	}

	bailout:
	return retval;
}