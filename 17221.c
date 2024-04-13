expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    int		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    int		cnt = 0;
    int		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    if (cmd == NULL)
	return -1;

#ifdef FEAT_EVAL
    if (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE);
    else
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    if (buffer == NULL)
	return -1;

    cmd = buffer;
    while (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	while (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	if (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	while (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    return cnt;
}