DLLEXPORT tjhandle DLLCALL tjInitTransform(void)
{
	tjinstance *this=NULL;  tjhandle handle=NULL;
	if((this=(tjinstance *)malloc(sizeof(tjinstance)))==NULL)
	{
		snprintf(errStr, JMSG_LENGTH_MAX,
			"tjInitTransform(): Memory allocation failure");
		return NULL;
	}
	MEMZERO(this, sizeof(tjinstance));
	snprintf(this->errStr, JMSG_LENGTH_MAX, "No error");
	handle=_tjInitCompress(this);
	if(!handle) return NULL;
	handle=_tjInitDecompress(this);
	return handle;
}