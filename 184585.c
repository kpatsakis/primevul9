jint JNICALL JVM_SendTo(jint descriptor, const char* buffer, jint length, jint flags, const struct sockaddr* toAddr, int toLength) {
	jint retVal;

	Trc_SC_SendTo_Entry(descriptor, buffer, length, flags, toAddr, toLength);

#if defined (WIN32)
	retVal = sendto(descriptor, buffer, length, flags, toAddr, toLength);
#else /* defined (WIN32) */
	do {
		retVal = sendto(descriptor, buffer, length, flags, toAddr, toLength);
	} while ((-1 == retVal) && (EINTR == errno));
#endif /* defined (WIN32) */

	Trc_SC_SendTo_Exit(retVal);

	return retVal;
}