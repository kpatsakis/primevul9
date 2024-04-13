deleteDirsFromLibpath(const char *const libpath, const char *const deleteStart, const size_t deleteLen)
{
	char *newPath = NULL;
	size_t preLen = deleteStart - libpath;
	const char *postStart = deleteStart + deleteLen;
	size_t postLen = strlen(postStart);
	size_t delim = 0;

	/* Remove trailing : from the prefix */
	while ((preLen > 0) && (':' == libpath[preLen - 1])) {
		preLen -= 1;
	}

	if (postLen > 0) {
		/* Remove leading : from the postfix */
		while (':' == postStart[0]) {
			postStart += 1;
			postLen -= 1;
		}
	}

	if ((preLen > 0) && (postLen > 0)) {
		/* Add delimiter : */
		delim = 1;
	}

	newPath = malloc(preLen + delim + postLen + 1);
	if (NULL == newPath) {
		fprintf(stderr, "deleteDirsFromLibpath: malloc(%d) failed, aborting\n", preLen + delim + postLen + 1);
		abort();
	}

	memcpy(newPath, libpath, preLen);
	if (delim > 0) {
		newPath[preLen] = ':';
	}

	memcpy(newPath + preLen + delim, postStart, postLen);

	/* Set the NUL terminator at the end */
	newPath[preLen + delim + postLen] = '\0';

	return newPath;
}