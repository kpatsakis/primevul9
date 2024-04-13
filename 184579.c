initializeWin32ThreadEvents(J9JavaVM* javaVM)
{
	J9VMThread* aThread;
	J9HookInterface** hookInterface = javaVM->internalVMFunctions->getVMHookInterface(javaVM);

	javaVM->sidecarInterruptFunction = bfuInterrupt;
	javaVM->sidecarClearInterruptFunction = bfuClearInterrupt;

	if ((*hookInterface)->J9HookRegisterWithCallSite(hookInterface, J9HOOK_VM_THREAD_CREATED, bfuThreadStart, OMR_GET_CALLSITE(), NULL)) {
		return JNI_ERR;
	}
	if ((*hookInterface)->J9HookRegisterWithCallSite(hookInterface, J9HOOK_VM_THREAD_DESTROY, bfuThreadEnd, OMR_GET_CALLSITE(), NULL)) {
		return JNI_ERR;
	}

	/* make sure that all existing threads gets an event, too */
	aThread = javaVM->mainThread;
	do {
		aThread->sidecarEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (aThread->sidecarEvent == NULL) {
			return JNI_ERR;
		}
		aThread = aThread->linkNext;
	} while (aThread != javaVM->mainThread);

	return JNI_OK;
}