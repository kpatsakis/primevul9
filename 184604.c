int findDirUplevelToDirContainingFile(J9StringBuffer **result, char *pathEnvar, char pathSeparator, char *fileInPath, int upLevels, int elementsToSkip) {
	char *paths;
	int   rc;

	/* Get the list of paths */
	paths = getenv(pathEnvar);
	if (!paths) {
		return FALSE;
	}

	/* find the directory */
	rc = findDirContainingFile(result, paths, pathSeparator, fileInPath, elementsToSkip);

	/* Now move upLevel to it - this may not work for directories of form
		/aaa/bbb/..      ... and so on.
		If that is a problem, could always use /.. to move up.
	*/
	if (rc) {
		for (; upLevels > 0; upLevels--) {
			truncatePath(jvmBufferData(*result));
		}
	}
   return rc;
}