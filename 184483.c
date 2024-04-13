JVM_SocketAvailable(jint descriptor, jint* result)
{
	jint retVal = 0;

	Trc_SC_SocketAvailable_Entry(descriptor, result);

#ifdef WIN32
	/* Windows JCL native doesn't invoke this JVM method */
	Assert_SC_unreachable();
#endif
#if defined(J9UNIX) || defined(J9ZOS390)
	if (0 <= descriptor) {
		do {
			retVal = ioctl(descriptor, FIONREAD, result);
		} while ((-1 == retVal) && (EINTR == errno));

		if (0 <= retVal) {
			/* ioctl succeeded, return 1 to indicate that this JVM method succeeds */
			retVal = 1;
		} else {
			/* ioctl failed, return 0 to indicate that this JVM method fails */
			retVal = 0;
		}
	}
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	Trc_SC_SocketAvailable_Exit(retVal, *result);

	return retVal;
}