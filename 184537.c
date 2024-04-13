JVM_ZipHook(JNIEnv *env, const char* filename, jint newState)
{
#ifdef J9VM_OPT_ZIP_SUPPORT
	VMI_ACCESS_FROM_ENV(env);
	J9JavaVM *vm = (J9JavaVM*)((J9VMThread*)env)->javaVM;
	J9HookInterface ** hook = (*VMI)->GetZipFunctions(VMI)->zip_getZipHookInterface(VMI);

	if (hook != NULL) {
		UDATA state;

		switch (newState) {
		case JVM_ZIP_HOOK_STATE_OPEN :
			state = J9ZIP_STATE_OPEN;
			break;
		case JVM_ZIP_HOOK_STATE_CLOSED :
			state = J9ZIP_STATE_CLOSED;
			break;
		case JVM_ZIP_HOOK_STATE_RESET :
			state = J9ZIP_STATE_RESET;
			break;
		default :
			state = 0;
		}
		/* Can't use hook trigger macros as can't include vmzipcachehook_internal.h */
		if (state) {
			struct J9VMZipLoadEvent eventData;

			eventData.portlib = vm->portLibrary;
			eventData.userData = vm;
			eventData.zipfile = NULL;
			eventData.newState = state;
			eventData.cpPath = (U_8*)filename;
			eventData.returnCode = 0;

			(*hook)->J9HookDispatch(hook, J9HOOK_VM_ZIP_LOAD, &eventData);
		}
	}
#endif /* J9VM_OPT_ZIP_SUPPORT */
}