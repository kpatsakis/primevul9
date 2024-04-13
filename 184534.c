jint JNICALL DestroyJavaVM(JavaVM * javaVM)
{
	jint rc;

	UT_MODULE_UNLOADED(J9_UTINTERFACE_FROM_VM((J9JavaVM*)javaVM));
	rc = globalDestroyVM(javaVM);

	if (JNI_OK == rc) {
#ifdef J9VM_OPT_HARMONY
		if ((NULL != harmonyPortLibrary) && (NULL != j9portLibrary.omrPortLibrary.mem_free_memory)) {
			j9portLibrary.omrPortLibrary.mem_free_memory(&j9portLibrary.omrPortLibrary, harmonyPortLibrary);
			harmonyPortLibrary = NULL;
		}
#endif /* J9VM_OPT_HARMONY */
		if (NULL != j9portLibrary.port_shutdown_library) {
			j9portLibrary.port_shutdown_library(&j9portLibrary);
		}

		freeGlobals();

#if defined(J9UNIX) || defined(J9ZOS390)
		j9vm_dllHandle = 0;
		java_dllHandle = 0;
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

		BFUjavaVM = NULL;
	} else {
		/* We are not shutting down the  port library but we still 
		 * need to make sure memcheck gets a chance to print its 
		 * report.
		 */
		memoryCheck_print_report(&j9portLibrary);
	}

	return rc;
}