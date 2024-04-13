static const char *token2str(int token)
{
	static const struct match_token *t;

	for (t = tokens; t->token != Opt_err; t++)
		if (t->token == token && !strchr(t->pattern, '='))
			break;
	return t->pattern;
}