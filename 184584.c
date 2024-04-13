JVM_LoadLibrary(const char *libName)
{
	JNIEnv *env;
	JavaVM *vm = (JavaVM *) BFUjavaVM;
	char errMsg[512];

#ifdef WIN32
	HINSTANCE dllHandle;
	UINT prevMode;

	Trc_SC_LoadLibrary_Entry(libName);

	prevMode = SetErrorMode( SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS );

	/* LoadLibrary will try appending .DLL if necessary */
	dllHandle = LoadLibrary ((LPCSTR)libName);
	if (NULL == dllHandle) {
		goto _end;
	}

	SetErrorMode(prevMode);
	Trc_SC_LoadLibrary_Exit(dllHandle);
	return dllHandle;

_end:
	SetErrorMode(prevMode);

#endif

#if defined(J9UNIX) || defined(J9ZOS390)
	void *dllHandle;
#if defined(AIXPPC)
	/* CMVC 137341:
	 * dlopen() searches for libraries using the LIBPATH envvar as it was when the process
	 * was launched.  This causes multiple issues such as:
	 *  - finding 32 bit binaries for libomrsig.so instead of the 64 bit binary needed and vice versa
	 *  - finding compressed reference binaries instead of non-compressed ref binaries
	 *
	 * calling loadAndInit(libname, 0 -> no flags, NULL -> use the currently defined LIBPATH) allows
	 * us to load the library with the current libpath instead of the one at process creation
	 * time. We can then call dlopen() as per normal and the just loaded library will be found.
	 * */
	loadAndInit((char *)libName, L_RTLD_LOCAL, NULL);
#endif
	dllHandle = dlopen( (char *)libName, RTLD_LAZY );
	if(NULL != dllHandle) {
		Trc_SC_LoadLibrary_Exit(dllHandle);
		return dllHandle;
	}
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	/* We are here means we failed to load library. Throw java.lang.UnsatisfiedLinkError */
 	(*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_2);
 	if (NULL != env) {
 		j9portLibrary.omrPortLibrary.str_printf(&j9portLibrary.omrPortLibrary, errMsg, sizeof(errMsg), "Failed to load library \"%s\"", libName);
 		throwNewUnsatisfiedLinkError(env, errMsg);
 	}

	Trc_SC_LoadLibrary_Exit(NULL);
	return NULL;
}