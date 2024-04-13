JVM_Open(const char* filename, jint flags, jint mode)
{
	int errorVal = 0;
	jint returnVal = 0;

	/* On OSX, stat64 and fstat64 are deprecated.
	 * Thus, stat and fstat are used on OSX.
	 */
#if defined(J9UNIX) && !defined(J9ZTPF) && !defined(OSX)
	struct stat64 tempStat;
	int doUnlink = 0;
#endif /* defined(J9UNIX) && !defined(J9ZTPF) && !defined(OSX) */
#if defined(J9ZOS390) || defined(J9ZTPF) || defined(OSX)
	struct stat tempStat;
	int doUnlink = 0;
#endif /* defined(J9ZOS390) || defined(J9ZTPF) || defined(OSX) */

	Trc_SC_Open_Entry(filename, flags, mode);

#define JVM_EEXIST -100

#ifdef WIN32
#ifdef __IBMC__
#define EXTRA_OPEN_FLAGS O_NOINHERIT | O_BINARY
#else
#define EXTRA_OPEN_FLAGS _O_NOINHERIT | _O_BINARY
#endif
#endif

#if defined(J9UNIX) || defined(J9ZOS390)
#if defined(OSX)
#define EXTRA_OPEN_FLAGS 0
#else
#define EXTRA_OPEN_FLAGS O_LARGEFILE
#endif /* defined(OSX) */

#ifndef O_DSYNC
#define O_DSYNC O_SYNC
#endif

	doUnlink = (flags & O_TEMPORARY);
#if !defined(J9ZTPF)
    flags &= (O_CREAT | O_APPEND | O_RDONLY | O_RDWR | O_TRUNC | O_WRONLY | O_EXCL | O_NOCTTY | O_NONBLOCK | O_NDELAY | O_SYNC | O_DSYNC);
#else /* !defined(J9ZTPF) */
    flags &= (O_CREAT | O_APPEND | O_RDONLY | O_RDWR | O_TRUNC | O_WRONLY | O_EXCL | O_NOCTTY | O_NONBLOCK | O_SYNC | O_DSYNC);
#endif /* !defined(J9ZTPF) */
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	/* For some reason, although JVM_NativePath is called on the filenames, some of them seem to
		get mangled between JVM_NativePath being called and JVM_open being called */
	filename = JVM_NativePath((char *)filename);

#if defined(J9UNIX) || defined(J9ZOS390)
	do {
		errorVal = 0;
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

#ifdef J9OS_I5
	returnVal = Xj9Open_JDK6((char *)filename, (flags | EXTRA_OPEN_FLAGS), mode);
#else
	returnVal = open(filename, (flags | EXTRA_OPEN_FLAGS), mode);
#endif
	if (-1 == returnVal) {
		errorVal = errno;
	}

#if defined(J9UNIX) || defined(J9ZOS390)
	/* Unix open() call does not reject directories, so extra checks required */
	if ((returnVal>=0) && (J9FSTAT(returnVal, &tempStat)==-1)) {
		Trc_SC_Open_fstat64(filename);
		close(returnVal);
		return -1;
	}
	if ((returnVal>=0) && S_ISDIR(tempStat.st_mode)) {
		char buf[1];

		Trc_SC_Open_isDirectory(filename);

		/* force errno to be EISDIR in case JVM_GetLastErrorString is called */
		errno = EISDIR;

		close(returnVal);
		return -1;
	}

	/* On unices, open() may return EAGAIN or EINTR, and should then be re-invoked */
	}
	while ((-1 == returnVal) && ((EAGAIN == errorVal) || (EINTR == errorVal)));

	/* Unix does not have an O_TEMPORARY flag. Unlink if Sovereign O_TEMPORARY flag passed in. */
	if ((returnVal>=0) && doUnlink)
		unlink(filename);
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	if (returnVal<0) {
		Trc_SC_Open_error(filename, errorVal);
	} else {
		Trc_SC_Open_Exit(filename, returnVal);
	}

	if (returnVal>=0)
		return returnVal;
	else if (EEXIST == errorVal)
		return JVM_EEXIST;
	else
		return -1;
}