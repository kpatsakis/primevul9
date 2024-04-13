DLLEXPORT int DLLCALL tjCompressFromYUV(tjhandle handle,
	const unsigned char *srcBuf, int width, int pad, int height, int subsamp,
	unsigned char **jpegBuf, unsigned long *jpegSize, int jpegQual, int flags)
{
	const unsigned char *srcPlanes[3];
	int pw0, ph0, strides[3], retval=-1;
	tjinstance *this=(tjinstance *)handle;

	if(!this) _throwg("tjCompressFromYUV(): Invalid handle");
	this->isInstanceError=FALSE;

	if(srcBuf==NULL || width<=0 || pad<1 || height<=0 || subsamp<0
		|| subsamp>=NUMSUBOPT)
		_throw("tjCompressFromYUV(): Invalid argument");

	pw0=tjPlaneWidth(0, width, subsamp);
	ph0=tjPlaneHeight(0, height, subsamp);
	srcPlanes[0]=srcBuf;
	strides[0]=PAD(pw0, pad);
	if(subsamp==TJSAMP_GRAY)
	{
		strides[1]=strides[2]=0;
		srcPlanes[1]=srcPlanes[2]=NULL;
	}
	else
	{
		int pw1=tjPlaneWidth(1, width, subsamp);
		int ph1=tjPlaneHeight(1, height, subsamp);
		strides[1]=strides[2]=PAD(pw1, pad);
		srcPlanes[1]=srcPlanes[0]+strides[0]*ph0;
		srcPlanes[2]=srcPlanes[1]+strides[1]*ph1;
	}

	return tjCompressFromYUVPlanes(handle, srcPlanes, width, strides, height,
		subsamp, jpegBuf, jpegSize, jpegQual, flags);

	bailout:
	return retval;
}