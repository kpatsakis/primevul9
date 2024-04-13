sread(int fd, void *buf, size_t n, int canbepipe)
{
	ssize_t rv;
#ifdef FIONREAD
	int t = 0;
#endif
	size_t rn = n;

	if (fd == STDIN_FILENO)
		goto nocheck;

#ifdef FIONREAD
	if (canbepipe && (ioctl(fd, FIONREAD, &t) == -1 || t == 0)) {
#ifdef FD_ZERO
		ssize_t cnt;
		for (cnt = 0;; cnt++) {
			fd_set check;
			struct timeval tout = {0, 100 * 1000};
			int selrv;

			FD_ZERO(&check);
			FD_SET(fd, &check);

			/*
			 * Avoid soft deadlock: do not read if there
			 * is nothing to read from sockets and pipes.
			 */
			selrv = select(fd + 1, &check, NULL, NULL, &tout);
			if (selrv == -1) {
				if (errno == EINTR || errno == EAGAIN)
					continue;
			} else if (selrv == 0 && cnt >= 5) {
				return 0;
			} else
				break;
		}
#endif
		(void)ioctl(fd, FIONREAD, &t);
	}

	if (t > 0 && (size_t)t < n) {
		n = t;
		rn = n;
	}
#endif

nocheck:
	do
		switch ((rv = FINFO_READ_FUNC(fd, buf, n))) {
		case -1:
			if (errno == EINTR)
				continue;
			return -1;
		case 0:
			return rn - n;
		default:
			n -= rv;
			buf = ((char *)buf) + rv;
			break;
		}
	while (n > 0);
	return rn;
}