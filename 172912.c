DLLEXPORT tjhandle DLLCALL tjInitDecompress(void)
{
	tjinstance *this;
	if((this=(tjinstance *)malloc(sizeof(tjinstance)))==NULL)
	{
		snprintf(errStr, JMSG_LENGTH_MAX,
			"tjInitDecompress(): Memory allocation failure");
		return NULL;
	}
	MEMZERO(this, sizeof(tjinstance));
	snprintf(this->errStr, JMSG_LENGTH_MAX, "No error");
	return _tjInitDecompress(this);
}