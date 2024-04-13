DLLEXPORT unsigned long DLLCALL tjBufSize(int width, int height,
	int jpegSubsamp)
{
	unsigned long retval=0;  int mcuw, mcuh, chromasf;
	if(width<1 || height<1 || jpegSubsamp<0 || jpegSubsamp>=NUMSUBOPT)
		_throwg("tjBufSize(): Invalid argument");

	/* This allows for rare corner cases in which a JPEG image can actually be
	   larger than the uncompressed input (we wouldn't mention it if it hadn't
	   happened before.) */
	mcuw=tjMCUWidth[jpegSubsamp];
	mcuh=tjMCUHeight[jpegSubsamp];
	chromasf=jpegSubsamp==TJSAMP_GRAY? 0: 4*64/(mcuw*mcuh);
	retval=PAD(width, mcuw) * PAD(height, mcuh) * (2 + chromasf) + 2048;

	bailout:
	return retval;
}