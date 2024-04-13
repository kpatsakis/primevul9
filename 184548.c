preloadLibrary(char* dllName, BOOLEAN inJVMDir)
{
	J9StringBuffer *buffer = NULL;
	void* handle = NULL;
#ifdef WIN32
	wchar_t unicodePath[J9_MAX_PATH];
	char * bufferData;
	size_t bufferLength;
#endif

	if(inJVMDir) {
		buffer = jvmBufferCat(buffer, jvmBufferData(j9binBuffer));
	} else {
		buffer = jvmBufferCat(buffer, jvmBufferData(jrebinBuffer));
	}
#ifdef WIN32
	buffer = jvmBufferCat(buffer, "\\");
	buffer = jvmBufferCat(buffer, dllName);
	buffer = jvmBufferCat(buffer, ".dll");
	bufferData = jvmBufferData(buffer);
	bufferLength = strlen(bufferData);
	MultiByteToWideChar(OS_ENCODING_CODE_PAGE, OS_ENCODING_MB_FLAGS, bufferData, -1, unicodePath, (int)bufferLength + 1);
	handle = (void*)LoadLibraryExW (unicodePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (handle == NULL) {
		fprintf(stderr,"jvm.dll preloadLibrary: LoadLibrary(%s) error: %x\n", buffer->data, GetLastError());
	}
#endif
#if defined(J9UNIX)
	buffer = jvmBufferCat(buffer, "/lib");
	buffer = jvmBufferCat(buffer, dllName);
	buffer = jvmBufferCat(buffer, J9PORT_LIBRARY_SUFFIX);
#ifdef AIXPPC
	loadAndInit(jvmBufferData(buffer), L_RTLD_LOCAL, NULL);
#endif
 	handle = (void*)dlopen(jvmBufferData(buffer), RTLD_NOW);
#ifdef AIXPPC
	if (handle == NULL) {
		int len = strlen(buffer->data);
		buffer->data[len - 2] = 'a';
		buffer->data[len - 1] = '\0';
		loadAndInit(buffer->data, L_RTLD_LOCAL, NULL);
		handle = (void*)dlopen(buffer->data, RTLD_NOW);
		if (handle == NULL) {
			/* get original error, otherwise the error displayed will be for a missing .a library. */
			buffer->data[len - 2] = 's';
			buffer->data[len - 1] = 'o';
			loadAndInit(buffer->data, L_RTLD_LOCAL, NULL);
			handle = (void*)dlopen(buffer->data, RTLD_NOW);
		}
	}
#endif /* AIXPPC */
	if (handle == NULL) {
		fprintf(stderr,"libjvm.so preloadLibrary(%s): %s\n", buffer->data, dlerror());
	}
#endif /* defined(J9UNIX) */
#ifdef J9ZOS390
	buffer = jvmBufferCat(buffer, "/lib");
	buffer = jvmBufferCat(buffer, dllName);
	buffer = jvmBufferCat(buffer, ".so");
	handle = (void*)dllload(jvmBufferData(buffer));
	if (handle == NULL) {
		perror("libjvm.so preloadLibrary: dllload() failed");
	}
#endif

	free(buffer);
	return handle;
}