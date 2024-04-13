JVM_Lseek(jint descriptor, jlong bytesToSeek, jint origin)
{
	jlong result = 0;
#if defined (LINUX)
	loff_t longResult = 0;
#endif

	Trc_SC_Lseek_Entry(descriptor, bytesToSeek, origin);

	if (descriptor == -1) {
		Trc_SC_Lseek_bad_descriptor();
		return JVM_IO_ERR;
	}

#if defined(WIN32)
#ifdef __IBMC__
	result = lseek(descriptor, (long) bytesToSeek, origin);
#else
	result = _lseeki64(descriptor, bytesToSeek, origin);
#endif
#elif defined(J9UNIX) || defined(J9ZOS390) /* defined(WIN32) */
#if defined(LINUX) && !defined(J9VM_ENV_DATA64)

#if __GLIBC_PREREQ(2,4)
	/* glibc 2.4 (sles 10) and on provide lseek64() */
	result = lseek64(descriptor, bytesToSeek, origin);
#else
	/* CMVC 104382:  Linux lseek uses native word size off_t so we need to use the 64-bit _llseek on 32-bit Linux.  AIX and z/OS use 64-bit API implicitly when we define _LARGE_FILES. */
	result = _llseek(descriptor, (unsigned long) ((bytesToSeek >> 32) & 0xFFFFFFFF), (unsigned long) (bytesToSeek & 0xFFFFFFFF), &longResult, origin);
	if (0 == result) {
		/* the call was successful so set the result to what we read */
		result = (jlong) longResult;
	}
#endif

#else /* defined(LINUX) && !defined(J9VM_ENV_DATA64) */
	result = lseek(descriptor, (off_t) bytesToSeek, origin);
#endif /* !defined(LINUX) ||defined(J9VM_ENV_DATA64) */
#else /* defined(WIN32) */
#error No JVM_Lseek provided
#endif /* defined(WIN32) */

	Trc_SC_Lseek_Exit(result);

	return result;
}