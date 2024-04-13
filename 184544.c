JVM_LoadSystemLibrary(const char *libName)
{
	JNIEnv *env;
	char errMsg[512];

#ifdef WIN32
	UDATA dllHandle;
	size_t libNameLen = strlen(libName);
	UDATA flags = 0;

	Trc_SC_LoadSystemLibrary_Entry(libName);

	if ((libNameLen <= 4) ||
			('.' != libName[libNameLen - 4]) ||
			('d' != j9_cmdla_tolower(libName[libNameLen - 3])) ||
			('l' != j9_cmdla_tolower(libName[libNameLen - 2])) ||
			('l' != j9_cmdla_tolower(libName[libNameLen - 1])))
	{
		flags = J9PORT_SLOPEN_DECORATE;
	}

	if (0 == j9util_open_system_library((char *)libName, &dllHandle, flags)) {
		Trc_SC_LoadSystemLibrary_Exit(dllHandle);
		return (void *)dllHandle;
	}

#endif

#if defined(J9UNIX) || defined(J9ZOS390)
	void *dllHandle;

	Trc_SC_LoadSystemLibrary_Entry(libName);

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
	if(NULL != dllHandle ) {
		Trc_SC_LoadSystemLibrary_Exit(dllHandle);
		return dllHandle;
	}

#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	/* We are here means we failed to load library. Throw java.lang.UnsatisfiedLinkError */
	Trc_SC_LoadSystemLibrary_LoadFailed(libName);

	if (NULL != BFUjavaVM) {
		JavaVM *vm = (JavaVM *) BFUjavaVM;
		(*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_2);
		if (NULL != env) {
			jio_snprintf(errMsg, sizeof(errMsg), "Failed to load library \"%s\"", libName);
			errMsg[sizeof(errMsg)-1] = '\0';
			throwNewUnsatisfiedLinkError(env, errMsg);
		}
	}
	Trc_SC_LoadSystemLibrary_Exit(NULL);

	return NULL;
}