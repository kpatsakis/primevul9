relpTcpWaitWriteable(relpTcp_t *const pThis, struct timespec *const tTimeout)
{
	int r;
	struct timespec tCurr; /* current time */
	struct pollfd pfd;

	clock_gettime(CLOCK_REALTIME, &tCurr);
	const int timeout =   (tTimeout->tv_sec - tCurr.tv_sec) * 1000
			    + (tTimeout->tv_nsec - tCurr.tv_nsec) / 1000000000;
	if(timeout < 0) {
		r = 0; goto done;
	}

	pThis->pEngine->dbgprint("librelp: telpTcpWaitWritable doing poll() "
		"on fd %d, timoeut %d\n", pThis->sock, timeout);

	pfd.fd = pThis->sock;
	pfd.events = POLLOUT;
	r = poll(&pfd, 1, timeout);
done:	return r;
}