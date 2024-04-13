JVM_Socket(jint domain, jint type, jint protocol)
{
	jint result;

	Trc_SC_Socket_Entry(domain, type, protocol);

#ifdef WIN32
	{
		SOCKET socketResult = socket(domain, type, protocol);
		SetHandleInformation((HANDLE)socketResult, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
		result = (jint)socketResult;
		Assert_SC_true(socketResult == (SOCKET)result);
	}
#else
	result = socket(domain, type, protocol);
#endif

	Trc_SC_Socket_Exit(result);

	return result;
}