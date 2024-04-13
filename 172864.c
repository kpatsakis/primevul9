DLLEXPORT unsigned long DLLCALL tjPlaneSizeYUV(int componentID, int width,
	int stride, int height, int subsamp)
{
	unsigned long retval=0;
	int pw, ph;

	if(width<1 || height<1 || subsamp<0 || subsamp>=NUMSUBOPT)
		_throwg("tjPlaneSizeYUV(): Invalid argument");

	pw=tjPlaneWidth(componentID, width, subsamp);
	ph=tjPlaneHeight(componentID, height, subsamp);
	if(pw<0 || ph<0) return -1;

	if(stride==0) stride=pw;
	else stride=abs(stride);

	retval=stride*(ph-1)+pw;

	bailout:
	return retval;
}