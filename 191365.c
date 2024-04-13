timeout_connect(int sockfd, const struct sockaddr *serv_addr,
    socklen_t addrlen, int *timeoutp)
{
	fd_set *fdset;
	struct timeval tv, t_start;
	socklen_t optlen;
	int optval, rc, result = -1;

	gettimeofday(&t_start, NULL);

	if (*timeoutp <= 0) {
		result = connect(sockfd, serv_addr, addrlen);
		goto done;
	}

	set_nonblock(sockfd);
	rc = connect(sockfd, serv_addr, addrlen);
	if (rc == 0) {
		unset_nonblock(sockfd);
		result = 0;
		goto done;
	}
	if (errno != EINPROGRESS) {
		result = -1;
		goto done;
	}

	fdset = (fd_set *)xcalloc(howmany(sockfd + 1, NFDBITS),
	    sizeof(fd_mask));
	FD_SET(sockfd, fdset);
	ms_to_timeval(&tv, *timeoutp);

	for (;;) {
		rc = select(sockfd + 1, NULL, fdset, NULL, &tv);
		if (rc != -1 || errno != EINTR)
			break;
	}

	switch (rc) {
	case 0:
		/* Timed out */
		errno = ETIMEDOUT;
		break;
	case -1:
		/* Select error */
		debug("select: %s", strerror(errno));
		break;
	case 1:
		/* Completed or failed */
		optval = 0;
		optlen = sizeof(optval);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval,
		    &optlen) == -1) {
			debug("getsockopt: %s", strerror(errno));
			break;
		}
		if (optval != 0) {
			errno = optval;
			break;
		}
		result = 0;
		unset_nonblock(sockfd);
		break;
	default:
		/* Should not occur */
		fatal("Bogus return (%d) from select()", rc);
	}

	free(fdset);

 done:
 	if (result == 0 && *timeoutp > 0) {
		ms_subtract_diff(&t_start, timeoutp);
		if (*timeoutp <= 0) {
			errno = ETIMEDOUT;
			result = -1;
		}
	}

	return (result);
}