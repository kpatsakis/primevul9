initialArgumentScan(JavaVMInitArgs *args, J9SpecialArguments *specialArgs) 
{
	BOOLEAN xCheckFound = FALSE;
	const char *xCheckString = "-Xcheck";
	const char *javaCommand = "-Dsun.java.command=";
	const char *javaCommandValue = NULL;
	const char *classPath = "-Djava.class.path=";
	const char *classPathValue = NULL;
	jint argCursor;
	UDATA argumentsSize = 0;

#ifdef WIN32
	Assert_SC_notNull(specialArgs->argEncoding);
#endif
	for (argCursor=0; argCursor < args->nOptions; argCursor++) {
		argumentsSize += strlen(args->options[argCursor].optionString) + 1; /* add space for the \0 */
		/* scan for -Xoss */
		if (0 == strncmp(args->options[argCursor].optionString, "-Xoss", 5)) {
			*(specialArgs->xoss) = argCursor;
		} else if (strncmp(args->options[argCursor].optionString, OPT_VERBOSE_INIT, strlen(OPT_VERBOSE_INIT))==0) {
			specialArgs->localVerboseLevel = VERBOSE_INIT;
		} else if (0 == strncmp(args->options[argCursor].optionString, javaCommand, strlen(javaCommand))) {
			javaCommandValue = args->options[argCursor].optionString + strlen(javaCommand);
		} else if (0 == strncmp(args->options[argCursor].optionString, classPath, strlen(classPath))) {
			classPathValue = args->options[argCursor].optionString + strlen(classPath);
		} else if (0 == strncmp(args->options[argCursor].optionString, xCheckString, strlen(xCheckString))) {
			xCheckFound = TRUE;
		} else if (0 == strcmp(args->options[argCursor].optionString, VMOPT_XARGENCODING)) {
			*(specialArgs->argEncoding) = ARG_ENCODING_PLATFORM;
		} else if (0 == strcmp(args->options[argCursor].optionString, VMOPT_XNOARGSCONVERSION)) {
			*(specialArgs->argEncoding) = ARG_ENCODING_LATIN;
		} else if (0 == strcmp(args->options[argCursor].optionString, VMOPT_XARGENCODINGUTF8)) {
			*(specialArgs->argEncoding) = ARG_ENCODING_UTF;
		} else if (0 == strcmp(args->options[argCursor].optionString, VMOPT_XARGENCODINGLATIN)) {
			*(specialArgs->argEncoding) = ARG_ENCODING_LATIN;
		}
	}

	if ((NULL != classPathValue) && (NULL != javaCommandValue) && (strcmp(javaCommandValue, classPathValue) == 0)) {
		specialArgs->executableJarPath = javaCommandValue;
	}

	if (TRUE == xCheckFound) {
		/* scan backwards for -Xcheck:memory.  There may be multiple -Xcheck options, so check them all, stop when we hit -Xcheck:memeory */
		for( argCursor = args->nOptions - 1 ; argCursor >= 0; argCursor-- ) {
			char* memcheckArgs[2];

			/* fake up command-line args for parseCmdLine */
			memcheckArgs[0] = ""; /* program name -- unused */
			memcheckArgs[1] = args->options[argCursor].optionString;
			if (memoryCheck_parseCmdLine(&j9portLibrary, 1, memcheckArgs) != 0) {
				/* Abandon -Xcheck processing */
				/* -Xcheck:memory overrides env var */
				*(specialArgs->ibmMallocTraceSet) = FALSE;
				break;
			}
		}
	}
	return argumentsSize;
}