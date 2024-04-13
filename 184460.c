jint JNICALL JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *vm_args) {
	J9JavaVM *j9vm = NULL;
	jint result = JNI_OK;
	IDATA xoss = -1;
	IDATA ibmMallocTraceSet = FALSE;
 	char cwd[J9_MAX_PATH];
#ifdef WIN32
	wchar_t unicodeTemp[J9_MAX_PATH];
	char *altLibraryPathBuffer = NULL;
#endif /* WIN32 */
	UDATA argEncoding = ARG_ENCODING_DEFAULT;
	UDATA altJavaHomeSpecified = 0; /* not used on non-Windows */
	J9PortLibraryVersion portLibraryVersion;
	UDATA j2seVersion;
#if defined(AIXPPC)
	char *origLibpath = NULL;
#endif /* AIXPPC */
	I_32 portLibraryInitStatus;
	UDATA expectedLibrarySize;
	UDATA localVerboseLevel = 0;
	J9CreateJavaVMParams createParams = {0};
	JavaVMInitArgs *args = NULL;
	UDATA launcherArgumentsSize = 0;
	J9VMInitArgs *j9ArgList = NULL;
	char *xServiceBuffer = NULL;
	const char *libpathValue = NULL;
	const char *ldLibraryPathValue = NULL;
	J9SpecialArguments specialArgs;
	omrthread_t attachedThread = NULL;
	specialArgs.localVerboseLevel = localVerboseLevel;
	specialArgs.xoss = &xoss;
	specialArgs.argEncoding = &argEncoding;
	specialArgs.executableJarPath = NULL;
	specialArgs.ibmMallocTraceSet = &ibmMallocTraceSet;
#ifdef J9ZTPF

    result = tpf_eownrc(TPF_SET_EOWNR, "IBMRT4J                        ");

    if (result < 0)  {
        fprintf(stderr, "Failed to issue tpf_eownrc.");
        return JNI_ERR;
    }
    result = tmslc(TMSLC_ENABLE+TMSLC_HOLD, "IBMRT4J");
    if (result < 0)  {
        fprintf(stderr, "Failed to start time slicing.");
        return JNI_ERR;
    }
#endif /* defined(J9ZTPF) */
	/*
	 * Linux uses LD_LIBRARY_PATH
	 * z/OS uses LIBPATH
	 * AIX uses both
	 * use native OS calls because port library isn't ready.
	 * Copy the strings because the value returned by getenv may change.
	 */
#if defined(AIXPPC) || defined(J9ZOS390)
	libpathValue = getenv(ENV_LIBPATH);
	if (NULL != libpathValue) {
		size_t pathLength = strlen(libpathValue) +1;
		char *envTemp = malloc(pathLength);
		if (NULL == envTemp) {
			result = JNI_ERR;
			goto exit;
		}
		strcpy(envTemp, libpathValue);
		libpathValue = envTemp;
	}
#endif
#if defined(J9UNIX)
	ldLibraryPathValue = getenv(ENV_LD_LIB_PATH);
	if (NULL != ldLibraryPathValue) {
		size_t pathLength = strlen(ldLibraryPathValue) +1;
		char *envTemp = malloc(pathLength);
		if (NULL == envTemp) {
			result = JNI_ERR;
			goto exit;
		}
		strcpy(envTemp, ldLibraryPathValue);
		ldLibraryPathValue = envTemp;
	}
#endif /* defined(J9UNIX) */

	if (BFUjavaVM != NULL) {
		result = JNI_ERR;
		goto exit;
	}

#ifdef J9OS_I5
	/* debug code */
	Xj9BreakPoint("jvm");
	/* Force iSeries create JVM flow */
	if (Xj9IleCreateJavaVmCalled() == 0) {
	    result = Xj9CallIleCreateJavaVm(pvm, penv, vm_args);
	    goto exit;
	}
#endif

#if defined(AIXPPC)
	/* CMVC 137180:
	 * in some cases the LIBPATH does not contain /usr/lib, when
	 * trying to load application native libraries that are linked against
	 * libraries in /usr/lib we could fail to find those libraries if /usr/lib
	 * is not on the LIBPATH.
	 * */
	addToLibpath("/usr/lib", FALSE);
	/* CMVC 135358.
	 * This function modifies LIBPATH while dlopen()ing J9 shared libs.
	 * Save the original so that it can be restored at the end of the
	 * function.
	 */
	origLibpath = getenv("LIBPATH");
#endif

	/* no tracing for this function, since it's unlikely to be used once the VM is running and the trace engine is initialized */
	preloadLibraries();
	
#ifdef WIN32
	if (GetCurrentDirectoryW(J9_MAX_PATH, unicodeTemp) == 0) {
		strcpy(cwd, "\\");
	} else {
		WideCharToMultiByte(OS_ENCODING_CODE_PAGE, OS_ENCODING_WC_FLAGS, unicodeTemp, -1,  cwd, J9_MAX_PATH, NULL, NULL);
	}
#else
	if (getcwd(cwd, J9_MAX_PATH) == NULL) {
		strcpy(cwd, ".");
	}
#endif
#ifdef DEBUG
	printf("cwd = %s\n", cwd);
#endif

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	if (0 != omrthread_attach_ex(&attachedThread, J9THREAD_ATTR_DEFAULT)) {
		result = JNI_ERR;
	}
#else /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	if (0 != f_threadAttachEx(&attachedThread, J9THREAD_ATTR_DEFAULT)) {
		result = JNI_ERR;
	}
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	if (JNI_OK != result) {
		goto exit;
	}

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	omrthread_lib_enable_cpu_monitor(attachedThread);
#else /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	f_libEnableCPUMonitor(attachedThread);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

#if defined(J9ZOS390)
	/*
	 * When we init the port lib, the 'Signal Reporter' thread will be spawned.
	 * On z/OS, we need to know whether this thread should be spawned as a medium 
	 * or heavy weight thread. We do this here but we will only take into
	 * account JAVA_THREAD_MODEL - i.e., if the customer is using '-Xthr:tw=heavy'
	 * instead of the env var, the 'Signal Reporter' thread will still be launched 
	 * as a medium weight thread (see PR100512). 
	 */
	if (!setZOSThrWeight()) {
		return JNI_ERR;
	}
#endif /* defined(J9ZOS390) */

	/* Use portlibrary version which we compiled against, and have allocated space
	 * for on the stack.  This version may be different from the one in the linked DLL.
	 */
	J9PORT_SET_VERSION(&portLibraryVersion, J9PORT_CAPABILITY_MASK);

	expectedLibrarySize = sizeof(J9PortLibrary);
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	portLibraryInitStatus = j9port_init_library(&j9portLibrary, &portLibraryVersion, expectedLibrarySize);
#else
	portLibraryInitStatus = portInitLibrary(&j9portLibrary, &portLibraryVersion, expectedLibrarySize);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

	if (0 != portLibraryInitStatus) {
		J9PortLibraryVersion actualVersion;
		/* port lib init failure */
		switch (portLibraryInitStatus) {
		case J9PORT_ERROR_INIT_WRONG_MAJOR_VERSION: {
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
			j9port_getVersion(&j9portLibrary, &actualVersion);
#else
			portGetVersionFn(&j9portLibrary, &actualVersion);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

			fprintf(stderr,"Error: Port Library failed to initialize: expected major version %u, actual version is %u\n",
					portLibraryVersion.majorVersionNumber, actualVersion.majorVersionNumber);
			break;
		}
		case J9PORT_ERROR_INIT_WRONG_SIZE: {
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
			UDATA actualSize = j9port_getSize(&portLibraryVersion);
#else
			UDATA actualSize = portGetSizeFn(&portLibraryVersion);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
			fprintf(stderr,"Error: Port Library failed to initialize: expected library size %" J9PRIz "u, actual size is %" J9PRIz "u\n",
					expectedLibrarySize, actualSize);
			break;
		}
		case J9PORT_ERROR_INIT_WRONG_CAPABILITIES: {
			fprintf(stderr,"Error: Port Library failed to initialize: capabilities do not match\n");
			break;
		}
		default: fprintf(stderr,"Error: Port Library failed to initialize: %i\n", portLibraryInitStatus); break;
		/* need this to handle legacy port libraries */
		}

#if defined(AIXPPC)
		/* Release memory if allocated by strdup() in backupLibpath() previously */
		freeBackupLibpath(&libpathBackup);

		/* restore LIBPATH to avoid polluting child processes */
		setLibpath(origLibpath);
#endif /* AIXPPC */
		result = JNI_ERR;
		goto exit;
	}

	/* Get the thread library memory categories */
	{
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		IDATA threadCategoryResult = omrthread_lib_control(J9THREAD_LIB_CONTROL_GET_MEM_CATEGORIES, (UDATA)&j9MasterMemCategorySet);
#else
		IDATA threadCategoryResult = f_threadLibControl(J9THREAD_LIB_CONTROL_GET_MEM_CATEGORIES, (UDATA)&j9MasterMemCategorySet);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

		if (threadCategoryResult) {
			fprintf(stderr,"Error: Couldn't get memory categories from thread library.\n");
#if defined(AIXPPC)
			/* Release memory if allocated by strdup() in backupLibpath() previously */
			freeBackupLibpath(&libpathBackup);

			/* restore LIBPATH to avoid polluting child processes */
			setLibpath(origLibpath);
#endif /* AIXPPC */
			result = JNI_ERR;
			goto exit;
		}
	}
	/* Register the J9 memory categories with the port library */
	j9portLibrary.omrPortLibrary.port_control(&j9portLibrary.omrPortLibrary, J9PORT_CTLDATA_MEM_CATEGORIES_SET, (UDATA)&j9MasterMemCategorySet);

	j2seVersion = getVersionFromPropertiesFile();
	if (J2SE_18 > j2seVersion) {
		fprintf(stderr, "Invalid version 0x%" J9PRIz "x detected in classlib.properties!\n", j2seVersion);
		result = JNI_ERR;
		goto exit;
	}
	setNLSCatalog(&j9portLibrary, j2seVersion);


#ifdef WIN32
	if (GetEnvironmentVariableW(IBM_MALLOCTRACE_STR, NULL, 0) > 0)
		ibmMallocTraceSet = TRUE;
	altJavaHomeSpecified = (GetEnvironmentVariableW(ALT_JAVA_HOME_DIR_STR, NULL, 0) > 0);
#endif
#if defined(J9UNIX) || defined(J9ZOS390)
	if (getenv(IBM_MALLOCTRACE_STR)) {
		ibmMallocTraceSet = TRUE;
	}
#endif /* defined(J9UNIX) || defined(J9ZOS390) */

	args = (JavaVMInitArgs *)vm_args;
	launcherArgumentsSize = initialArgumentScan(args, &specialArgs);
	localVerboseLevel = specialArgs.localVerboseLevel;

	if (VERBOSE_INIT == localVerboseLevel) {
		createParams.flags |= J9_CREATEJAVAVM_VERBOSE_INIT;
	}

	if (ibmMallocTraceSet) {
		/* We have no access to the original command line, so cannot
		 * pass in a valid argv to this function.
		 * Currently this function only used argv to help set the NLS
		 * catalog.
		 * The catalog has already been set above. */
		memoryCheck_initialize(&j9portLibrary, "all", NULL);
	}
	
	{
		char *optionsDefaultFileLocation = NULL;
		BOOLEAN doAddExtDir = FALSE;
		J9JavaVMArgInfoList vmArgumentsList;
		J9ZipFunctionTable *zipFuncs = NULL;
		vmArgumentsList.pool = pool_new(sizeof(J9JavaVMArgInfo),
				32, /* expect at least ~16 arguments, overallocate to accommodate user arguments */
				0, 0,
				J9_GET_CALLSITE(), OMRMEM_CATEGORY_VM, POOL_FOR_PORT(&j9portLibrary));
		if (NULL == vmArgumentsList.pool) {
			result = JNI_ERR;
			goto exit;
		}
		vmArgumentsList.head = NULL;
		vmArgumentsList.tail = NULL;
		if (NULL != specialArgs.executableJarPath) {
#if !CALL_BUNDLED_FUNCTIONS_DIRECTLY
			if (NULL == f_j9_GetInterface) {
#ifdef WIN32
				f_j9_GetInterface = (J9GetInterface) GetProcAddress (j9vm_dllHandle, (LPCSTR) "J9_GetInterface");
#else
				f_j9_GetInterface = (J9GetInterface) dlsym (j9vm_dllHandle, "J9_GetInterface");
#endif /* WIN32 */
			}
			/* j9binBuffer->data is null terminated */
			zipFuncs = (J9ZipFunctionTable*) f_j9_GetInterface(IF_ZIPSUP, &j9portLibrary, j9binBuffer->data);
#else /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
			zipFuncs = (J9ZipFunctionTable*) J9_GetInterface(IF_ZIPSUP, &j9portLibrary, j9binBuffer);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
		}
		if ((j2seVersion & J2SE_SERVICE_RELEASE_MASK) >= J2SE_19) {
			optionsDefaultFileLocation = jvmBufferData(j9libBuffer);
		} else {
			optionsDefaultFileLocation = jvmBufferData(j9binBuffer);
			doAddExtDir = TRUE;
		}
		/* now add implicit VM arguments */
		if (
				/* Add the default options file */
				(0 != addOptionsDefaultFile(&j9portLibrary, &vmArgumentsList, optionsDefaultFileLocation, localVerboseLevel))
				|| (0 != addXjcl(&j9portLibrary, &vmArgumentsList, j2seVersion))
				|| (0 != addBootLibraryPath(&j9portLibrary, &vmArgumentsList, "-Dcom.ibm.oti.vm.bootstrap.library.path=",
						jvmBufferData(j9binBuffer), jvmBufferData(jrebinBuffer)))
				|| (0 != addBootLibraryPath(&j9portLibrary, &vmArgumentsList, "-Dsun.boot.library.path=",
						jvmBufferData(j9binBuffer), jvmBufferData(jrebinBuffer)))
				|| (0 != addJavaLibraryPath(&j9portLibrary, &vmArgumentsList, argEncoding, jvmInSubdir,
						jvmBufferData(j9binBuffer), jvmBufferData(jrebinBuffer),
						libpathValue, ldLibraryPathValue))
				|| (0 != addJavaHome(&j9portLibrary, &vmArgumentsList, altJavaHomeSpecified, jvmBufferData(j9libBuffer)))
				|| (doAddExtDir && (0 != addExtDir(&j9portLibrary, &vmArgumentsList, jvmBufferData(j9libBuffer), args, j2seVersion)))
				|| (0 != addUserDir(&j9portLibrary, &vmArgumentsList, cwd))
				|| (0 != addJavaPropertiesOptions(&j9portLibrary, &vmArgumentsList, localVerboseLevel))
				|| (0 != addJarArguments(&j9portLibrary, &vmArgumentsList, specialArgs.executableJarPath, zipFuncs, localVerboseLevel))
				|| (0 != addEnvironmentVariables(&j9portLibrary, args, &vmArgumentsList, localVerboseLevel))
				|| (0 != addLauncherArgs(&j9portLibrary, args, launcherArgumentsSize, &vmArgumentsList,
						&xServiceBuffer, argEncoding, localVerboseLevel))
#ifdef J9VM_OPT_HARMONY
				/* pass in the Harmony library */
				|| (0 != addHarmonyPortLibrary(&j9portLibrary, &vmArgumentsList, localVerboseLevel))
#endif /* J9VM_OPT_HARMONY */
				|| (0 != addXserviceArgs(&j9portLibrary, &vmArgumentsList, xServiceBuffer, localVerboseLevel))
		) {
			result = JNI_ERR;
			goto exit;
		}

		if (NULL != libpathValue) {
			free((void *)libpathValue);
		}
		if (NULL != ldLibraryPathValue) {
			free((void *)ldLibraryPathValue);
		}
		j9ArgList = createJvmInitArgs(&j9portLibrary, args, &vmArgumentsList, &argEncoding);
		if (ARG_ENCODING_LATIN == argEncoding) {
			createParams.flags |= J9_CREATEJAVAVM_ARGENCODING_LATIN;
		} else if (ARG_ENCODING_UTF == argEncoding) {
			createParams.flags |= J9_CREATEJAVAVM_ARGENCODING_UTF8;
		} else if (ARG_ENCODING_PLATFORM == argEncoding) {
			createParams.flags |= J9_CREATEJAVAVM_ARGENCODING_PLATFORM;
		}

		pool_kill(vmArgumentsList.pool);
		if (NULL == j9ArgList) {
			result = JNI_ERR;
			goto exit;
		}
	}

	if(jvmInSubdir) {
		j2seVersion |= J2SE_LAYOUT_VM_IN_SUBDIR;
	}

	createParams.j2seVersion = j2seVersion;
	createParams.j2seRootDirectory = jvmBufferData(j9binBuffer);
	createParams.j9libvmDirectory = jvmBufferData(j9libvmBuffer);

	createParams.portLibrary = &j9portLibrary;
	createParams.globalJavaVM = &BFUjavaVM;

	if (VERBOSE_INIT == localVerboseLevel) {
		fprintf(stderr, "VM known paths\t- j9libvm directory: %s\n\t\t- j2seRoot directory: %s\n",
			createParams.j9libvmDirectory,
			createParams.j2seRootDirectory);

		printVmArgumentsList(j9ArgList);
	}
	createParams.vm_args = j9ArgList;

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	result = J9_CreateJavaVM((JavaVM**)&BFUjavaVM, penv, &createParams);
#else 
	result = globalCreateVM((JavaVM**)&BFUjavaVM, penv, &createParams);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY  */

#ifdef DEBUG
	fprintf(stdout,"Finished, result %d, env %llx\n", result, (long long)*penv);
	fflush(stdout);
#endif
	if (result == JNI_OK) {
		BOOLEAN initializeReflectAccessors = TRUE;
		JavaVM * vm = (JavaVM*)BFUjavaVM;
		*pvm = vm;

#if !defined(HARMONY_VM)
		/* Initialize the Sun VMI */
		ENSURE_VMI();
#endif

		memcpy(&globalInvokeInterface, *vm, sizeof(J9InternalVMFunctions));
		globalDestroyVM = globalInvokeInterface.DestroyJavaVM;
		globalInvokeInterface.DestroyJavaVM = DestroyJavaVM;
		issueWriteBarrier();
		*vm = (struct JNIInvokeInterface_ *) &globalInvokeInterface;

#if !defined(HARMONY_VM)
		/* Harmony Select does not want any of the Win32 threading or reflection globals init. */

#ifdef WIN32
		result = initializeWin32ThreadEvents(BFUjavaVM);
		if (result != JNI_OK) {
			(**pvm)->DestroyJavaVM(*pvm);
			goto exit;
		}
#endif

		/* Initialize the VM interface */
		result = initializeReflectionGlobals(*penv, initializeReflectAccessors);
		if (result != JNI_OK) {
			(**pvm)->DestroyJavaVM(*pvm);
			goto exit;
		}
#endif /* !defined(HARMONY_VM) */

	} else {
		freeGlobals();
	}

	if ((result == JNI_OK) && (BFUjavaVM->runtimeFlags & J9_RUNTIME_SHOW_VERSION)) {
		JNIEnv * env = *penv;
		jclass clazz = (*env)->FindClass(env, "sun/misc/Version");

		if (clazz == NULL) {
			(*env)->ExceptionClear(env);
		} else {
			jmethodID mid = (*env)->GetStaticMethodID(env, clazz, "print", "()V");

			if (mid != NULL) {
				(*env)->CallStaticVoidMethod(env, clazz, mid);
				if (!(*env)->ExceptionCheck(env)) {
					j9portLibrary.omrPortLibrary.tty_printf(&j9portLibrary.omrPortLibrary, "\n");
				}
			}
			(*env)->ExceptionClear(env);
			(*env)->DeleteLocalRef(env, clazz);
		}
	}

#if defined(AIXPPC)
	/* restore LIBPATH to avoid polluting child processes */
	restoreLibpath(&libpathBackup);
#ifdef DEBUG_TEST
	testBackupAndRestoreLibpath();
#endif /* DEBUG_TEST */
#endif /* AIXPPC */

	if (JNI_OK == result) {
		J9JavaVM *env = (J9JavaVM *) BFUjavaVM;
		J9VMThread *currentThread = env->mainThread;
		omrthread_t thread = currentThread->osThread;
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		omrthread_set_category(thread, J9THREAD_CATEGORY_APPLICATION_THREAD, J9THREAD_TYPE_SET_MODIFY);
#else
		f_setCategory(thread, J9THREAD_CATEGORY_APPLICATION_THREAD, J9THREAD_TYPE_SET_MODIFY);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	}

exit:
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	if (NULL != attachedThread) {
		omrthread_detach(attachedThread);
	}
#else /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	if (NULL != attachedThread) {
		f_threadDetach(attachedThread);
	}
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

	return result;
}