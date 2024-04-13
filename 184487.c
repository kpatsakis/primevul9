preloadLibraries(void)
{
	void *vmDLL, *threadDLL, *portDLL;
	char* lastSep = 0;
	char* tempchar = 0;
	char* exeInBuf;
	J9StringBuffer * jvmDLLNameBuffer = NULL;
	char *lastDirName;
	char* vmDllName = J9_VM_DLL_NAME;
	struct stat statBuf;
#if defined(J9ZOS390)
	void *javaDLL;
#endif

#if defined(AIXPPC)
	size_t origLibpathLen = 0;
	const char *origLibpath = NULL;
#endif /* AIXPPC */

	if (j9vm_dllHandle != 0) {
		return FALSE;
	}

#if defined(J9ZOS390)
	iconv_init();
#endif

#if defined(AIXPPC)
	origLibpath = getenv("LIBPATH");
	if (NULL != origLibpath) {
		origLibpathLen = strlen(origLibpath);
	}
#endif /* AIXPPC */

	jvmDLLNameBuffer = getj9bin();
	j9binBuffer = jvmBufferCat(NULL, jvmBufferData(jvmDLLNameBuffer));

	addToLibpath(jvmBufferData(j9binBuffer), TRUE);

	/* Are we in classic? If so, point to the right arch dir. */
	lastDirName = strrchr(jvmBufferData(j9binBuffer), DIR_SEPARATOR);

	if (NULL == lastDirName) {
		fprintf(stderr, "Preload libraries failed to find a valid J9 binary location\n" );
		exit( -1 ); /* failed */
	}

	if (0 == strcmp(lastDirName + 1, "classic")) {
		truncatePath(jvmBufferData(j9binBuffer)); /* at jre/bin or jre/lib/<arch> */
		truncatePath(jvmBufferData(j9binBuffer)); /* at jre     or jre/lib        */
#if J9VM_JAVA9_BUILD < 150
		/* remove /lib if present */
		removeSuffix(jvmBufferData(j9binBuffer), "/lib"); /* at jre */
#endif /* J9VM_JAVA9_BUILD < 150 */
		j9binBuffer = jvmBufferCat(j9binBuffer, J9VM_LIB_ARCH_DIR "j9vm/");
		if (-1 != stat(jvmBufferData(j9binBuffer), &statBuf)) {
			/* does exist, carry on */
			jvmDLLNameBuffer = jvmBufferCat(NULL, jvmBufferData(j9binBuffer));
		} else {
			/* does not exist, rewind (likely a 5.0 build) */
			free(j9binBuffer);
			j9binBuffer = NULL;
			j9binBuffer = jvmBufferCat(NULL, jvmBufferData(jvmDLLNameBuffer));
		}
	}

	/* detect if we're in a subdir or not */
	jvmDLLNameBuffer = jvmBufferCat(jvmDLLNameBuffer, "/lib");
	jvmDLLNameBuffer = jvmBufferCat(jvmDLLNameBuffer, vmDllName);
	jvmDLLNameBuffer = jvmBufferCat(jvmDLLNameBuffer, J9PORT_LIBRARY_SUFFIX);

	if(-1 != stat (jvmBufferData(jvmDLLNameBuffer), &statBuf)) {
		jvmInSubdir = TRUE;
	} else {
		jvmInSubdir = FALSE;
	}

	DBG_MSG(("jvmInSubdir: %d\n", jvmInSubdir));

	free(jrebinBuffer);
	jrebinBuffer = NULL;

	/* set up jre bin based on result of subdir knowledge */
	if(jvmInSubdir) {
		jrebinBuffer = jvmBufferCat(NULL, jvmBufferData(j9binBuffer));
		truncatePath(jvmBufferData(jrebinBuffer));
	} else {
		truncatePath(jvmBufferData(j9binBuffer));
		jrebinBuffer = jvmBufferCat(NULL, jvmBufferData(j9binBuffer));
	}
	j9libBuffer = jvmBufferCat(NULL, jvmBufferData(jrebinBuffer));
#if !defined(OSX)
	/* <arch> directory doesn't exist on OSX so j9libBuffer shouldn't
	 * be truncated on OSX for removing <arch>.
	 */
#if (J9VM_JAVA9_BUILD < 150)
	/* Remove <arch> */
	truncatePath(jvmBufferData(j9libBuffer));
#endif /* (J9VM_JAVA9_BUILD < 150) */
#endif /* !defined(OSX) */
	j9libvmBuffer = jvmBufferCat(NULL, jvmBufferData(j9binBuffer));
	j9Buffer = jvmBufferCat(NULL, jvmBufferData(jrebinBuffer));
	truncatePath(jvmBufferData(j9Buffer));

	DBG_MSG(("j9binBuffer   = <%s>\n", jvmBufferData(j9binBuffer)));
	DBG_MSG(("jrebinBuffer  = <%s>\n", jvmBufferData(jrebinBuffer)));
	DBG_MSG(("j9libBuffer   = <%s>\n", jvmBufferData(j9libBuffer)));
	DBG_MSG(("j9libvmBuffer = <%s>\n", jvmBufferData(j9libvmBuffer)));
	DBG_MSG(("j9Buffer      = <%s>\n", jvmBufferData(j9Buffer)));

	addToLibpath(jvmBufferData(jrebinBuffer), TRUE);

#if defined(AIXPPC)
	backupLibpath(&libpathBackup, origLibpathLen);
#endif /* AIXPPC */

	omrsigDLL = preloadLibrary("omrsig", TRUE);
	if (NULL == omrsigDLL) {
		fprintf(stderr, "libomrsig failed to load: omrsig\n" );
		exit( -1 ); /* failed */
	}

	vmDLL = preloadLibrary(vmDllName, TRUE);
	if (NULL == vmDLL) {
		fprintf(stderr,"libjvm.so failed to load: %s\n", vmDllName);
		exit( -1 );	/* failed */
	}

	globalCreateVM = (CreateVM) dlsym (vmDLL, CREATE_JAVA_VM_ENTRYPOINT );
	globalGetVMs = (GetVMs) dlsym (vmDLL,  GET_JAVA_VMS_ENTRYPOINT);
	if ((NULL == globalCreateVM) || (NULL == globalGetVMs)) {
		dlclose(vmDLL);
		fprintf(stderr,"libjvm.so failed to load: global entrypoints not found\n");
		exit( -1 );	/* failed */
	}
	j9vm_dllHandle = vmDLL;

#ifdef J9ZOS390
	/* pre-load libjava.so for IMBZOS functions */
	javaDLL = preloadLibrary("java", FALSE);
	if (!javaDLL) {
	   fprintf(stderr,"libjava.dll failed to load: %s\n", "java");
	   exit( -1 );     /* failed */
	}
	globalGetStringPlatform = (pGetStringPlatform) dlsym (javaDLL,  "IBMZOS_GetStringPlatform");
	globalGetStringPlatformLength = (pGetStringPlatformLength) dlsym (javaDLL,  "IBMZOS_GetStringPlatformLength");
	globalNewStringPlatform = (pNewStringPlatform) dlsym (javaDLL,  "IBMZOS_NewStringPlatform");
	global_a2e_vsprintf = (p_a2e_vsprintf) dlsym (javaDLL,  "IBMZOS_a2e_vsprintf");
	if (!globalGetStringPlatform || !globalGetStringPlatformLength || !globalNewStringPlatform || !global_a2e_vsprintf) {
	   dlclose(vmDLL);
	   dlclose(javaDLL);
	   fprintf(stderr,"libjava.dll failed to load: global entrypoints not found\n");
	   exit( -1 );     /* failed */
	}
	java_dllHandle = javaDLL;
#endif

	threadDLL = preloadLibrary(J9_THREAD_DLL_NAME, TRUE);
	f_threadGlobal = (ThreadGlobal) dlsym (threadDLL, "omrthread_global");
	f_threadAttachEx = (ThreadAttachEx) dlsym (threadDLL, "omrthread_attach_ex");
	f_threadDetach = (ThreadDetach) dlsym (threadDLL, "omrthread_detach");
	f_monitorEnter = (MonitorEnter) dlsym (threadDLL, "omrthread_monitor_enter");
	f_monitorExit = (MonitorExit) dlsym (threadDLL, "omrthread_monitor_exit");
	f_monitorInit = (MonitorInit) dlsym (threadDLL, "omrthread_monitor_init_with_name");
	f_monitorDestroy = (MonitorDestroy) dlsym (threadDLL, "omrthread_monitor_destroy");
	f_monitorWaitTimed = (MonitorWaitTimed) dlsym (threadDLL, "omrthread_monitor_wait_timed");
	f_monitorNotify = (MonitorNotify) dlsym (threadDLL, "omrthread_monitor_notify");
	f_monitorNotifyAll = (MonitorNotifyAll) dlsym (threadDLL, "omrthread_monitor_notify_all");
	f_threadLibControl = (ThreadLibControl) dlsym (threadDLL, "omrthread_lib_control");
	f_setCategory = (SetCategory) dlsym (threadDLL, "omrthread_set_category");
	f_libEnableCPUMonitor = (LibEnableCPUMonitor) dlsym (threadDLL, "omrthread_lib_enable_cpu_monitor");
	if (!f_threadGlobal || !f_threadAttachEx || !f_threadDetach || !f_monitorEnter || !f_monitorExit || !f_monitorInit || !f_monitorDestroy || !f_monitorWaitTimed
		|| !f_monitorNotify || !f_monitorNotifyAll || !f_threadLibControl || !f_setCategory || !f_libEnableCPUMonitor) {
		dlclose(vmDLL);
#ifdef J9ZOS390
		dlclose(javaDLL);
#endif
		dlclose(threadDLL);
		fprintf(stderr,"libjvm.so failed to load: thread library entrypoints not found\n");
		exit( -1 );	/* failed */
	}
	portDLL = preloadLibrary(J9_PORT_DLL_NAME, TRUE);
	portInitLibrary = (PortInitLibrary) dlsym (portDLL, "j9port_init_library");
	portGetSizeFn = (PortGetSize) dlsym (portDLL, "j9port_getSize");
	portGetVersionFn = (PortGetVersion) dlsym (portDLL, "j9port_getVersion");
	if (!portInitLibrary) {
		dlclose(vmDLL);
#ifdef J9ZOS390
		dlclose(javaDLL);
#endif
		dlclose(threadDLL);
		dlclose(portDLL);
		fprintf(stderr,"libjvm.so failed to load: %s entrypoints not found\n", J9_PORT_DLL_NAME);
		exit( -1 );	/* failed */
	}

#ifdef J9_CLEAR_VM_INTERFACE_DLL_NAME
	/* CMVC 142575: Harmony JDWP sits apart for the JVM natives including the vmi.  
	 * We must preload the library so that it can be found when JDWP tries to load it. */
	preloadLibrary(J9_CLEAR_VM_INTERFACE_DLL_NAME, TRUE);
#endif /* J9_CLEAR_VM_INTERFACE_DLL_NAME */

#ifdef J9VM_OPT_HARMONY
	/* CMVC 152702: with other JVM on the path this library can get loaded from the wrong
	 * location if not preloaded. */
	preloadLibrary(J9_HARMONY_PORT_LIBRARY_SHIM_DLL_NAME, TRUE);
#endif /* J9VM_OPT_HARMONY */
	return TRUE;
}