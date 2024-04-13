DLLEXPORT int tjPlaneHeight(int componentID, int height, int subsamp)
{
	int ph, nc, retval=0;

	if(height<1 || subsamp<0 || subsamp>=TJ_NUMSAMP)
		_throwg("tjPlaneHeight(): Invalid argument");
	nc=(subsamp==TJSAMP_GRAY? 1:3);
	if(componentID<0 || componentID>=nc)
		_throwg("tjPlaneHeight(): Invalid argument");

	ph=PAD(height, tjMCUHeight[subsamp]/8);
	if(componentID==0)
		retval=ph;
	else
		retval=ph*8/tjMCUHeight[subsamp];

	bailout:
	return retval;
}