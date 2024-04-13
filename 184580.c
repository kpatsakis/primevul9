JNI_GetCreatedJavaVMs(JavaVM **vmBuf, jsize bufLen, jsize *nVMs)
{
	jint result;
	Trc_SC_GetCreatedJavaVMs_Entry(vmBuf, bufLen, nVMs);

	/* for proxy we cannot preload the libraries as we don't know whether proxy is enabled or not at this point
	 * However, if the libraries have not already been preloaded then we know that no vms have been created and
	 * can return the right answer
	 */
	if(librariesLoaded()){
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		result = J9_GetCreatedJavaVMs(vmBuf, bufLen, nVMs);
#else
		result = globalGetVMs(vmBuf, bufLen, nVMs);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	} else {
		/* simply return that there have been no JVMs created */
		result = JNI_OK;
		*nVMs = 0;
	}

	Trc_SC_GetCreatedJavaVMs_Exit(result, *nVMs);
	return result;
}