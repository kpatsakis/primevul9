static int parse_cred_line(char *line, char **target)
{
	if (line == NULL || target == NULL)
		goto parsing_err;

	/* position target at first char of value */
	*target = strchr(line, '=');
	if (!*target)
		goto parsing_err;
	*target += 1;

	/* tell the caller which value target points to */
	if (strncasecmp("user", line, 4) == 0)
		return CRED_USER;
	if (strncasecmp("pass", line, 4) == 0)
		return CRED_PASS;
	if (strncasecmp("dom", line, 3) == 0)
		return CRED_DOM;

parsing_err:
	return CRED_UNPARSEABLE;
}