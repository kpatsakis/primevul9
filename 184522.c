JVM_RegisterSignal(jint sigNum, void *handler)
{
	void *oldHandler = (void *)J9_SIG_ERR;
	J9JavaVM *javaVM = (J9JavaVM *)BFUjavaVM;
	J9InternalVMFunctions *vmFuncs = javaVM->internalVMFunctions;
	J9VMThread *currentThread = vmFuncs->currentVMThread(javaVM);
	BOOLEAN isShutdownSignal = isSignalUsedForShutdown(sigNum);
	BOOLEAN isSignalIgnored = FALSE;
	int32_t isSignalIgnoredError = 0;
	uint32_t portlibSignalFlag = 0;

	PORT_ACCESS_FROM_JAVAVM(javaVM);

	Trc_SC_RegisterSignal_Entry(currentThread, sigNum, handler);

	portlibSignalFlag = j9sig_map_os_signal_to_portlib_signal(sigNum);
	if (0 != portlibSignalFlag) {
		isSignalIgnoredError = j9sig_is_signal_ignored(portlibSignalFlag, &isSignalIgnored);
	}

	if (isSignalReservedByJVM(sigNum)) {
		/* Return error if signal is reserved by the JVM. oldHandler is initialized to
		 * J9_SIG_ERR.
		 */
		goto exit;
	} else if (J9_ARE_ANY_BITS_SET(javaVM->sigFlags, J9_SIG_XRS_ASYNC) && isShutdownSignal) {
		/* Don't register a handler for shutdown signals if -Xrs or -Xrs:async is specified.
		 * If -Xrs:sync is specified, then register a handler for shutdown signals. oldHandler
		 * is initialized to J9_SIG_ERR.
		 */
		goto exit;
	} else if (isShutdownSignal && ((0 == isSignalIgnoredError) && isSignalIgnored)) {
		/* Ignore shutdown signal if it is ignored by the OS. */
		oldHandler = (void *)J9_SIG_IGNORED;
		goto exit;
	} else {
		/* Register the signal. */
		IDATA isHandlerRegistered = 0;
		if ((void *)J9_PRE_DEFINED_HANDLER_CHECK == handler) {
			isHandlerRegistered = vmFuncs->registerPredefinedHandler(javaVM, sigNum, &oldHandler);
		} else {
			isHandlerRegistered = vmFuncs->registerOSHandler(javaVM, sigNum, handler, &oldHandler);
		}
		if (0 != isHandlerRegistered) {
			Trc_SC_RegisterSignal_FailedToRegisterHandler(currentThread, sigNum, handler, oldHandler);
		}
	}

	/* If oldHandler is a master handler, then a Java signal handler was previously registered with
	 * the signal. sun.misc.Signal.handle(...) or jdk.internal.misc.Signal.handle(...) will return
	 * the old Java signal handler if JVM_RegisterSignal returns J9_USE_OLD_JAVA_SIGNAL_HANDLER.
	 * Otherwise, an instance of NativeHandler is returned with the oldHandler's address stored in
	 * NativeHandler.handler. In Java 8, NativeHandler.handle() will invoke JVM_RegisterSignal using
	 * NativeHandler.handler, which represents the address of the native signal handler function. In
	 * Java 9, NativeHandler.handle() will throw UnsupportedOperationException.
	 */
	if (j9sig_is_master_signal_handler(oldHandler)) {
		oldHandler = (void *)J9_USE_OLD_JAVA_SIGNAL_HANDLER;
	}

exit:
	Trc_SC_RegisterSignal_Exit(currentThread, oldHandler);
	return oldHandler;
}