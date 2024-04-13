DLLEXPORT tjhandle DLLCALL tjInitCompress(void)
{
	tjinstance *this=NULL;
	if((this=(tjinstance *)malloc(sizeof(tjinstance)))==NULL)
	{
		snprintf(errStr, JMSG_LENGTH_MAX,
			"tjInitCompress(): Memory allocation failure");
		return NULL;
	}
	MEMZERO(this, sizeof(tjinstance));
	snprintf(this->errStr, JMSG_LENGTH_MAX, "No error");
	return _tjInitCompress(this);
}