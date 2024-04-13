JVM_GetLastErrorString(char* buffer, jint length)
{
	int savedErrno = errno;
#ifdef WIN32
	DWORD errorCode = GetLastError();
#endif
	jint retVal = 0;

	Trc_SC_GetLastErrorString_Entry(buffer, length);

	memset(buffer, 0, length);

#ifdef WIN32
	if (errorCode) {
		retVal = formatErrorMessage(errorCode, buffer, length);
	} else
#endif
	if (savedErrno) {
		PORT_ACCESS_FROM_JAVAVM(BFUjavaVM);
		I_32 sigRC;
		UDATA fnRC;

		sigRC = j9sig_protect(protectedStrerror, (void *) (IDATA) savedErrno, strerrorSignalHandler, NULL, J9PORT_SIG_FLAG_SIGALLSYNC | J9PORT_SIG_FLAG_MAY_RETURN, &fnRC);
		if (sigRC == 0) {
			strncat(buffer, (char *) fnRC, (length-1));
			/* casting to jint is safe since we know that the result is <= length */
			retVal = (jint)strlen(buffer);
		}
	}

	Trc_SC_GetLastErrorString_Exit(retVal, buffer);

	return retVal;
}