static char *timelib_string(Scanner *s)
{
	char *tmp = timelib_calloc(1, s->cur - s->tok + 1);
	memcpy(tmp, s->tok, s->cur - s->tok);

	return tmp;
}