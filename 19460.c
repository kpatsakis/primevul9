static void pregexp(const char *prog, int flags)
{
	pc('/');
	while (*prog) {
		if (*prog == '/')
			pc('\\');
		pc(*prog);
		++prog;
	}
	pc('/');
	if (flags & JS_REGEXP_G) pc('g');
	if (flags & JS_REGEXP_I) pc('i');
	if (flags & JS_REGEXP_M) pc('m');
}