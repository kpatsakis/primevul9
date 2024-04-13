addToLibpath(const char *dir, BOOLEAN isPrepend)
{
#if defined(AIXPPC) || defined(J9ZOS390)
	char *oldPath, *newPath;
	int rc, newSize;
#if defined(J9ZOS390)
	char *putenvPath;
	int putenvSize;
#endif

	oldPath = getenv("LIBPATH");
#ifdef DEBUG
	printf("\nLIBPATH before = %s\n", oldPath ? oldPath : "<empty>");
#endif
	newSize = (oldPath ? strlen(oldPath) : 0) + strlen(dir) + 2;  /* 1 for :, 1 for \0 terminator */
	newPath = malloc(newSize);

	if(!newPath) {
		fprintf(stderr, "addToLibpath malloc(%d) 1 failed, aborting\n", newSize);
		abort();
	}
#if defined(AIXPPC)
	if (oldPath) {
		if (isPrepend) {
			strcpy(newPath, dir);
			strcat(newPath, ":");
			strcat(newPath, oldPath);
		} else {
			strcpy(newPath, oldPath);
			strcat(newPath, ":");
			strcat(newPath, dir);
		}
	} else {
		strcpy(newPath, dir);
	}

#else
	/* ZOS doesn't like it when we pre-pend to LIBPATH */
	if (oldPath) {
		strcpy(newPath, oldPath);
		strcat(newPath, ":");
	} else {
		newPath[0] = '\0';
	}
	strcat(newPath, dir);
#endif

#if defined(J9ZOS390)
	putenvSize = newSize + strlen("LIBPATH=");
	putenvPath = malloc(putenvSize);
	if(!putenvPath) {
		fprintf(stderr, "addToLibpath malloc(%d) 2 failed, aborting\n", putenvSize);
		abort();
	}

	strcpy(putenvPath,"LIBPATH=");
	strcat(putenvPath, newPath);
	rc = putenv(putenvPath);
	free(putenvPath);
#else
	rc = setenv("LIBPATH", newPath, 1);
#endif

#ifdef DEBUG
	printf("\nLIBPATH after = %s\n", getenv("LIBPATH"));
#endif
	free(newPath);
#endif
}