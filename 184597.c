JVM_SetLength(jint fd, jlong length)
{
	jint result;

	Trc_SC_SetLength_Entry(fd, length);

	if (fd == -1) {
		Trc_SC_SetLength_bad_descriptor();
		return -1;
	}

#if defined(WIN32_IBMC)
	printf("_JVM_SetLength@12 called but not yet implemented. Exiting.");
	exit(43);
#elif defined(WIN32) /* defined(WIN32_IBMC) */
	result = _chsize(fd, (long)length);
#elif defined(J9UNIX) && !defined(J9ZTPF) && !defined(OSX) /* defined(WIN32_IBMC) */
	result = ftruncate64(fd, length);
#elif defined(J9ZOS390) || defined(J9ZTPF) || defined(OSX) /* defined(WIN32_IBMC) */
	/* ftruncate64 is unsupported on OSX. */
	result = ftruncate(fd, length);
#else /* defined(WIN32_IBMC) */
#error "Please provide an implementation of jvm.c:JVM_SetLength(jint fd, jlong length)"
#endif /* defined(WIN32_IBMC) */

	Trc_SC_SetLength_Exit(result);

	return result;
}