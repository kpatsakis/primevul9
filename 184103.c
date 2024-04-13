static char **crunch_list(char *list)
{
	int count, i;
	char *p, *q;
	char **result = NULL;

	p = list;
	
	/* strip off trailing delimiters */
	while (p[strlen(p)-1] == LIST_DELIMITER) {
		count--;
		p[strlen(p)-1] = '\0';
	}
	/* cut off leading delimiters */
	while (p[0] == LIST_DELIMITER) {
		count--;
		p++; 
	}
	
	/* count delimiters to calculate elements */
	for (count=i=0; p[i]; i++)
		if (p[i] == LIST_DELIMITER) count++;
	
	if ((result = (char **)malloc(sizeof(char *) * (count+2))) == NULL) {
		printf ("Sorry, can't get enough memory, exiting.\n");
		exit(0); /* safe exit, because only called during startup */
	}
	
	/*
	 * We now can assume that the first and last
	 * characters are different from any delimiters,
	 * so we don't have to care about this.
	 */
	count = 0;
	while ((q=strchr(p, LIST_DELIMITER))) {
		result[count] = (char *) malloc((q - p + 1) * sizeof(char));
		if (result[count] == NULL) {
			printf ("Sorry, can't get enough memory, exiting.\n");
			exit(0); /* safe exit, because only called during startup */
		}
		strncpy(result[count], p, q - p);
		result[count][q - p] = '\0';
		p = q; p++;
		count++;
	}
	if ((result[count] = \
	     (char *)malloc(sizeof(char) * strlen(p) + 1)) == NULL) {
		printf ("Sorry, can't get enough memory, exiting.\n");
		exit(0); /* safe exit, because only called during startup */
	}
	strcpy(result[count],p);
	result[++count] = NULL;

#if 0
	count=0;
	while (result[count])
		DBGPRINTF("#%d: %s\n", count, StripDomains[count++]);
#endif
	return result;
}