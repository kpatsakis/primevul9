xmlShellReadline(char *prompt) {
#ifdef HAVE_LIBREADLINE
    char *line_read;

    /* Get a line from the user. */
    line_read = readline (prompt);

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
	add_history (line_read);

    return (line_read);
#else
    char line_read[501];
    char *ret;
    int len;

    if (prompt != NULL)
	fprintf(stdout, "%s", prompt);
    fflush(stdout);
    if (!fgets(line_read, 500, stdin))
        return(NULL);
    line_read[500] = 0;
    len = strlen(line_read);
    ret = (char *) malloc(len + 1);
    if (ret != NULL) {
	memcpy (ret, line_read, len + 1);
    }
    return(ret);
#endif
}