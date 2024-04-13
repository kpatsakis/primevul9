JVM_Connect(jint descriptor, const struct sockaddr*address, int length)
{
	jint retVal;

	Trc_SC_Connect_Entry(descriptor, address, length);

#if defined (WIN32)
	retVal = connect(descriptor, address, length);
#else /* defined (WIN32) */
	do {
		retVal = connect(descriptor, address, length);
	} while ((-1 == retVal) && (EINTR == errno));
#endif /* defined (WIN32) */

	Trc_SC_Connect_Exit(retVal);

	return retVal;
}