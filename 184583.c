jint JNICALL JVM_Listen(jint descriptor, jint count) {
	jint retVal;

	Trc_SC_Listen_Entry(descriptor, count);

	retVal = listen(descriptor, count);

	Trc_SC_Listen_Exit(retVal);

	return retVal;
}