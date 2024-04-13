truncatePath(char *inputPath) {
	char *lastOccurence = strrchr(inputPath, DIR_SEPARATOR);
	/* strrchr() returns NULL if it cannot find the character */
	if (NULL != lastOccurence) {
		*lastOccurence = '\0';
	}
}