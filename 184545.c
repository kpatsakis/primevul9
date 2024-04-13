exitHook(J9JavaVM *vm)
{
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	while (omrthread_monitor_enter(vm->vmThreadListMutex), vm->sidecarExitFunctions)
#else
	while (f_monitorEnter(vm->vmThreadListMutex), vm->sidecarExitFunctions)
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	{
		J9SidecarExitFunction * current = vm->sidecarExitFunctions;

		vm->sidecarExitFunctions = current->next;
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		omrthread_monitor_exit(vm->vmThreadListMutex);
#else
		f_monitorExit(vm->vmThreadListMutex);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
		current->func();
		free(current);
	}

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	omrthread_monitor_exit(vm->vmThreadListMutex);
#else
	f_monitorExit(vm->vmThreadListMutex);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
}