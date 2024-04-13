JVM_Timeout(jint descriptor, jint timeout)
{
	jint result = 0;
	struct timeval tval;
#ifdef WIN32
	struct fd_set fdset;
#endif

#if defined(J9UNIX) || defined(J9ZOS390)
	jint returnVal = 0; 
	jint crazyCntr = 10;
	fd_set fdset;
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	Trc_SC_Timeout_Entry(descriptor, timeout);

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	FD_ZERO(&fdset);
	FD_SET((u_int)descriptor, &fdset);
#if defined(WIN32)
	result = select(0, &fdset, 0, 0, &tval);
#elif defined(J9ZTPF) /* defined(WIN32) */
        if (-1 == timeout)  {
                result = select(0, &fdset, 0, 0, NULL);
        } else  {
                result = select(0, &fdset, 0, 0, &tval);
        }
#elif defined(J9UNIX) || defined(J9ZOS390) /* defined(WIN32) */
	do {
		crazyCntr--;
		returnVal = select(descriptor+1, &fdset, 0, 0, &tval);
		if (returnVal==1 && !FD_ISSET((u_int)descriptor, &fdset)) {
			result = 0;
			break;
		}
		if (!(returnVal<0 && errno==EINTR)) {
			result = returnVal;
			break;
		}
	} while (crazyCntr);
#endif /* defined(WIN32) */

	Trc_SC_Timeout_Exit(result);

	return result;
}