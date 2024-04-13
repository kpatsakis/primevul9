JVM_Close(jint descriptor)
{
	jint result = 0;

	Trc_SC_Close_Entry(descriptor);

	if (descriptor == -1) {
		Trc_SC_Close_bad_descriptor();
		return JVM_IO_ERR;
	}

	/* Ignore close of stdin, when
	 * JVM_Read/Write mapped to use the port library
	 * api/java_io/FileInputStream/index.html#CtorFD
	 * also ignore close of stdout and stderr */

	if (descriptor >= 0 && descriptor <= 2) {
		Trc_SC_Close_std_descriptor();
		return 0;
	}

	result = close(descriptor);

	Trc_SC_Close_Exit(result);

	return result;
}