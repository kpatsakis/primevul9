JVM_RecvFrom(jint descriptor, char* buffer, jint length, jint flags, struct sockaddr* fromAddr, int* fromLength)
{
	jint retVal;

	Trc_SC_RecvFrom_Entry(descriptor, buffer, length, flags, fromAddr, fromLength);

#if defined (WIN32)
	retVal = recvfrom(descriptor, buffer, length, flags, fromAddr, fromLength);
#else
	{
		socklen_t address_len = (socklen_t)*fromLength;
		do {
			retVal = recvfrom(descriptor, buffer, (size_t)length, flags, fromAddr, &address_len);
		} while ((-1 == retVal) && (EINTR == errno));
		*fromLength = (int)address_len;
	}
#endif

	Trc_SC_RecvFrom_Exit(retVal, *fromLength);

	return retVal;
}