JVM_Recv(jint descriptor, char* buffer, jint length, jint flags)
{
	jint retVal;

#ifdef AIXPPC
	int returnVal=0;
	fd_set fdset;
	struct timeval tval;
#endif

	Trc_SC_Recv_Entry(descriptor, buffer, length, flags);

#ifdef AIXPPC
	tval.tv_sec = 1;
	tval.tv_usec = 0;

	do {
		FD_ZERO(&fdset);
		FD_SET((u_int)descriptor, &fdset);

		returnVal = select(descriptor+1, &fdset, 0, 0, &tval);
	} while(returnVal == 0);
#endif

#ifdef WIN32
	retVal = recv(descriptor, buffer, (int)length, flags);
#else
	do {
		retVal = recv(descriptor, buffer, (size_t)length, flags);
	} while ((-1 == retVal) && (EINTR == errno));
#endif

	Trc_SC_Recv_Exit(retVal);

	return retVal;
}