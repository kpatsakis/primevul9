JVM_Accept(jint descriptor, struct sockaddr* address, int* length)
{
	jint retVal;

	Trc_SC_Accept_Entry(descriptor, address, length);

#if defined(AIXPPC)
	{
		int returnVal=0;
		fd_set fdset;
		struct timeval tval;
		socklen_t socklen = (socklen_t)*length;

		tval.tv_sec = 1;
		tval.tv_usec = 0;

		do {
			FD_ZERO(&fdset);
			FD_SET((u_int)descriptor, &fdset);

			returnVal = select(descriptor+1, &fdset, 0, 0, &tval);
		} while(returnVal == 0);

		do {
			retVal = accept(descriptor, address, &socklen);
		} while ((-1 == retVal) && (EINTR == errno));

		*length = (int)socklen;
	}
#elif defined (WIN32)
	{
		SOCKET socketResult = accept(descriptor, address, length);
		retVal = (jint)socketResult;
		Assert_SC_true(socketResult == (SOCKET)retVal);
	}
#else
	{
		socklen_t socklen = (socklen_t)*length;
		do {
			retVal = accept(descriptor, address, &socklen);
		} while ((-1 == retVal) && (EINTR == errno));
		*length = (int)socklen;
	}
#endif

	Trc_SC_Accept_Exit(retVal, *length);

	return retVal;
}