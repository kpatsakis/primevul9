JVM_OnExit(void (*func)(void))
{
	J9SidecarExitFunction * newFunc;

	Trc_SC_OnExit_Entry(func);

	newFunc = (J9SidecarExitFunction *) malloc(sizeof(J9SidecarExitFunction));
	if (newFunc) {
		newFunc->func = func;

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		omrthread_monitor_enter(BFUjavaVM->vmThreadListMutex);
#else
		f_monitorEnter(BFUjavaVM->vmThreadListMutex);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

		newFunc->next = BFUjavaVM->sidecarExitFunctions;
		BFUjavaVM->sidecarExitFunctions = newFunc;

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		omrthread_monitor_exit(BFUjavaVM->vmThreadListMutex);
#else
		f_monitorExit(BFUjavaVM->vmThreadListMutex);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

		BFUjavaVM->sidecarExitHook = &exitHook;
	} else {
		Trc_SC_OnExit_OutOfMemory();
	}

	Trc_SC_OnExit_Exit(newFunc);
}