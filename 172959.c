DLLEXPORT char* DLLCALL tjGetErrorStr2(tjhandle handle)
{
	tjinstance *this=(tjinstance *)handle;
	if(this && this->isInstanceError)
	{
		this->isInstanceError=FALSE;
		return this->errStr;
	}
	else return errStr;
}