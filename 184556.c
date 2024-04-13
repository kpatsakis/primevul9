JVM_Available(jint descriptor, jlong* bytes)
{
	jlong curr = 0;
	jlong end = 0;

	/* On OSX, stat64 and fstat64 are deprecated.
	 * Thus, stat and fstat are used on OSX.
	 */
#if defined(J9UNIX) && !defined(J9ZTPF) && !defined(OSX)
	struct stat64 tempStat;
#endif /* defined(J9UNIX) && !defined(J9ZTPF) && !defined(OSX) */
#if defined(J9ZOS390) || defined(J9ZTPF) || defined(OSX)
	struct stat tempStat;
#endif /* defined(J9ZOS390) || defined(J9ZTPF) || defined(OSX) */
#if defined(LINUX)
	loff_t longResult = 0;
#endif

	Trc_SC_Available_Entry(descriptor, bytes);

	if (descriptor == -1) {
		Trc_SC_Available_bad_descriptor();
		*bytes = 0;
		return JNI_FALSE;
	}

#if defined(WIN32) && !defined(__IBMC__)
	curr = _lseeki64(descriptor, 0, SEEK_CUR);
	if (curr==-1L) {
		if (descriptor == 0) {
			/* Failed on stdin, check input queue. */
			DWORD result = 0;
			if (GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &result)) {
				*bytes = result;
			} else {
				*bytes = 0;
			}
			Trc_SC_Available_Exit(1, *bytes);
			return 1;
		}
		Trc_SC_Available_lseek_failed(descriptor);
		return 0;		/* Returning zero causes JNU_ThrowIOExceptionWithLastError() */
	}
	end = _lseeki64(descriptor, 0, SEEK_END);
	_lseeki64(descriptor, curr, SEEK_SET);
#else
		if (J9FSTAT(descriptor, &tempStat) == -1) {
			Trc_SC_Available_fstat_failed(descriptor, errno);
			*bytes = 0;
			return 0;
		}
		else if (S_ISFIFO(tempStat.st_mode) || S_ISSOCK(tempStat.st_mode) || S_ISCHR(tempStat.st_mode)) {
			/*
				arg3 is the third argument to ioctl.  The type of this argument is dependent upon the information
				being requested.  FIONREAD specifies that the argument be a pointer to an int
			*/
			int arg3FIONREAD = 0;

#if defined(J9ZOS390)
			/* CMVC 100066:  FIONREAD on z/OS only works for sockets so we need to use fstat, instead */
			if (!S_ISSOCK(tempStat.st_mode)) {
				/* we already performed the stat, so just return the st_size */
				*bytes = tempStat.st_size;
				Trc_SC_Available_Exit(1, *bytes);
				return 1;
			}
#endif
			if (ioctl(descriptor, FIONREAD, &(arg3FIONREAD)) == -1) {
				if (descriptor == 0) {
					/* Failed on stdin, just return st_size. */
					*bytes = tempStat.st_size;
					Trc_SC_Available_Exit(1, *bytes);
					return 1;
				}
#if (defined(LINUX) && !defined(J9ZTPF)) || defined(AIXPPC) || defined(J9ZOS390)
				else {
					struct pollfd pollOne;
					int  ret = 0;
					pollOne.fd = descriptor;
					pollOne.events = POLLRDNORM | POLLRDBAND | POLLPRI;
					pollOne.revents = 0;
					if (-1 != poll(&pollOne, 1, 0)) {
						/* poll succeeded (-1 is failure) */
						if(0 != (pollOne.events & pollOne.revents)) {
							/* if the one descriptor we were looking at returns a modified revents
									which matches a read operation, return at least one byte readable */
							*bytes = 1;
							Trc_SC_Available_Exit(1, *bytes);
							return 1;
						} else {
							/* none of the events are ready so this is 0 bytes readable */
							*bytes = 0;
							Trc_SC_Available_Exit(1, *bytes);
							return 1;
						}
					} else {
						/* poll failed so use a poll failure trace point and return failure */
						Trc_SC_Available_poll_failed(descriptor, errno);
						*bytes = 0;
						return 0;
					}
				}
#endif
				Trc_SC_Available_ioctl_failed(descriptor, errno);
				*bytes = 0;
				return 0;
			}

			*bytes = (jlong) arg3FIONREAD;
			Trc_SC_Available_Exit(1, *bytes);
			return 1;
		}
#if defined(LINUX) && !defined(J9VM_ENV_DATA64)
#if __GLIBC_PREREQ(2,4)
	/* glibc 2.4 (sles 10) and on provide lseek64() */
	curr = lseek64(descriptor, 0, SEEK_CUR);
#else
	/* CMVC 104382:  Linux lseek uses native word size off_t so we need to use the 64-bit _llseek on 32-bit Linux.  AIX and z/OS use 64-bit API implicitly when we define _LARGE_FILES. */
	curr = _llseek(descriptor, 0, 0, &longResult, SEEK_CUR);
	if (0 == curr) {
		/* the call was successful so set the result to what we read */
		curr = (jlong) longResult;
	}
#endif
#else	/* defined(LINUX) && !defined(J9VM_ENV_DATA64) */
	curr = lseek(descriptor, 0, SEEK_CUR);
#endif /*!defined (LINUX) || defined(J9VM_ENV_DATA64) */
	if (curr==-1L) {
		if (descriptor == 0) {
			/* Failed on stdin, just return 0. */
			*bytes = 0;
			Trc_SC_Available_Exit(1, *bytes);
			return 1;
		}
		Trc_SC_Available_lseek_failed(descriptor);
		return 0;		/* Returning zero causes JNU_ThrowIOExceptionWithLastError() */
	}

	end = tempStat.st_size;     /* size from previous fstat                                      */
#endif

	*bytes = (end-curr);

	Trc_SC_Available_Exit(1, *bytes);

	return 1;
}