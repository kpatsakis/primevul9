JVM_RaiseSignal(jint sigNum)
{
	jboolean rc = JNI_FALSE;
	J9JavaVM *javaVM = (J9JavaVM *)BFUjavaVM;
	BOOLEAN isShutdownSignal = isSignalUsedForShutdown(sigNum);
	BOOLEAN isSignalIgnored = FALSE;
	int32_t isSignalIgnoredError = 0;
	uint32_t portlibSignalFlag = 0;

	PORT_ACCESS_FROM_JAVAVM(javaVM);

	Trc_SC_RaiseSignal_Entry(sigNum);

	portlibSignalFlag = j9sig_map_os_signal_to_portlib_signal(sigNum);
	if (0 != portlibSignalFlag) {
		isSignalIgnoredError = j9sig_is_signal_ignored(portlibSignalFlag, &isSignalIgnored);
	}

	if (isSignalReservedByJVM(sigNum)) {
		/* Don't raise a signal if it is reserved by the JVM, and not
		 * registered via JVM_RegisterSignal.
		 */
	} else if (J9_ARE_ALL_BITS_SET(javaVM->sigFlags, J9_SIG_XRS_ASYNC) && isShutdownSignal) {
		/* Ignore shutdown signals if -Xrs or -Xrs:async is specified.
		 * If -Xrs:sync is specified, then raise shutdown signals.
		 */
	} else if (isShutdownSignal && ((0 == isSignalIgnoredError) && isSignalIgnored)) {
		/* Ignore shutdown signal if it is ignored by the OS. */
	} else {
		raise(sigNum);
		rc = JNI_TRUE;
	}

	Trc_SC_RaiseSignal_Exit(rc);

	return rc;
}