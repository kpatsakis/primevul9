file_regexec(file_regex_t *rx, const char *str, size_t nmatch,
    regmatch_t* pmatch, int eflags)
{
	assert(rx->rc == 0);
	return regexec(&rx->rx, str, nmatch, pmatch, eflags);
}