getj9bin()
{
	/* misnamed - returns the directory that the jvm DLL is found in, NOT the directory that the J9 VM itself is in. */

	J9StringBuffer *result = NULL;
#if (defined(LINUX) && !defined(J9ZTPF)) || defined(OSX)
	Dl_info libraryInfo;
	int rc = dladdr((void *)getj9bin, &libraryInfo);

	if (0 == rc) {
		fprintf(stderr, "ERROR: cannot determine JAVA home directory\n");
		abort();
	}

	result = jvmBufferCat(NULL, libraryInfo.dli_fname);
	/* remove libjvm.so */
	truncatePath(jvmBufferData(result));
#elif defined(J9ZOS390) || defined(J9ZTPF)
#define VMDLL_NAME J9_VM_DLL_NAME

	int foundPosition = 0;

	/* assumes LIBPATH (or LD_LIBRARY_PATH for z/TPF) points to where all libjvm.so can be found */
	while(foundPosition = findDirUplevelToDirContainingFile(&result, LD_ENV_PATH, ':', "libjvm.so", 0, foundPosition)) {
		/* now screen to see if match is the right libjvm.so - it needs to have a j9vm DLL either in this dir, or in the parent. */
		DBG_MSG(("found a libjvm.so at offset %d - looking at elem: %s\n", foundPosition, result));

		/* first try this dir - this will be true for 'vm in subdir' cases, and is the likely Java 6 case as of SR1. */
		if (isFileInDir(jvmBufferData(result), "lib" VMDLL_NAME J9PORT_LIBRARY_SUFFIX)) {
			return result;
		}

		truncatePath(jvmBufferData(result));

		/* trying parent */
		if (isFileInDir(jvmBufferData(result), "lib" VMDLL_NAME J9PORT_LIBRARY_SUFFIX)) {
			return result;
		}
	}

	fprintf(stderr, "ERROR: cannot determine JAVA home directory\n");
	abort();

#else /* must be AIX / RS6000 */
	struct ld_info *linfo, *linfop;
	int             linfoSize, rc;
	char           *myAddress, *filename, *membername;

	/* get loader information */
	linfoSize = 1024;
	linfo = malloc(linfoSize);
	for(;;) {
		rc = loadquery(L_GETINFO, linfo, linfoSize);
		if (rc != -1) {
			break;
		}
		linfoSize *=2; /* insufficient buffer size - increase */
		linfo = realloc(linfo, linfoSize);
	}

	/* find entry for my loaded object */
	myAddress = ((char **)&getj9bin)[0];
	for (linfop = linfo;;) {
		char *textorg  = (char *)linfop->ldinfo_textorg;
		char *textend  = textorg + (unsigned long)linfop->ldinfo_textsize;
		if (myAddress >=textorg && (myAddress < textend)) {
			break;
		}
		if (!linfop->ldinfo_next) {
			abort();
		}
		linfop = (struct ld_info *)((char *)linfop + linfop->ldinfo_next);
	}

	filename   = linfop->ldinfo_filename;
	membername = filename+strlen(filename)+1;
#ifdef DEBUG
	printf("ldinfo: filename is %s. membername is %s\n",  filename, membername);
#endif

	result = jvmBufferCat(NULL, filename);
	/* remove '/libjvm.a' */
	truncatePath(jvmBufferData(result));

	free(linfo);
#endif  /* defined(LINUX) && !defined(J9ZTPF) */
	return result;
}