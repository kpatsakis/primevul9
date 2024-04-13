static int token_pass_cmp(const void *tp0p, const void *tp1p)
{
	const struct token_pass * const tp0 = (const struct token_pass * const)tp0p;
	const struct token_pass * const tp1 = (const struct token_pass * const)tp1p;
	int rc;

	if (!tp1->token || !tp0->token)
		return tp1->token - tp0->token;
	rc = strcmp(tp0->token, tp1->token);
	if (rc == 0)
		rc = strcmp(tp0->pass, tp1->pass);
	return rc;
}