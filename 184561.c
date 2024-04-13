JVM_Read(jint descriptor, char *buffer, jint bytesToRead)
{
	PORT_ACCESS_FROM_JAVAVM(BFUjavaVM);
	jint result = 0;

	Trc_SC_Read_Entry(descriptor, buffer, bytesToRead);

	if (descriptor == -1) {
		Trc_SC_Read_bad_descriptor();
		return -1;
	}

#ifndef J9ZOS390
	/* This code SHOULD be for Windows only, but is safe everywhere except z/OS
	 * (see CMVC 99667 for the fun details of z/OS and POSIX File-handles):
	 * Map stdin to the port library, so we avoid any CR/LF translation.
	 * See JVM_WRITE
	 */
	if (descriptor == 0) {
		IDATA charsRead = j9tty_get_chars(buffer, bytesToRead);
		result = (jint)charsRead;
		Assert_SC_true(charsRead == (IDATA)result);
	} else
#endif
	{
		/* CMVC 178203 - Restart system calls interrupted by EINTR */
		do {
			result = read(descriptor, buffer, bytesToRead);
		} while ((-1 == result) && (EINTR == errno));
	}

	Trc_SC_Read_Exit(result, errno);

	return result;
}