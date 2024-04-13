JVM_SocketClose(jint descriptor)
{
	jint retVal;

	Trc_SC_SocketClose_Entry(descriptor);

	if (descriptor<=0) {
		Trc_SC_SocketClose_bad_descriptor();
		return 1;
	}

#if defined(WIN32)
	(void)shutdown(descriptor, SD_SEND);
	(void)closesocket(descriptor);
	retVal = 1; /* Always return TRUE */
#else
	do {
		retVal = close(descriptor);
	} while ((-1 == retVal) && (EINTR == errno));
#endif

	Trc_SC_SocketClose_Exit(retVal);

	return retVal;
}