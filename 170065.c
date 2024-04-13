static void add_pbf_warning(Scanner *s, char *error, char *sptr, char *cptr)
{
	s->errors->warning_count++;
	s->errors->warning_messages = timelib_realloc(s->errors->warning_messages, s->errors->warning_count * sizeof(timelib_error_message));
	s->errors->warning_messages[s->errors->warning_count - 1].position = cptr - sptr;
	s->errors->warning_messages[s->errors->warning_count - 1].character = *cptr;
	s->errors->warning_messages[s->errors->warning_count - 1].message = timelib_strdup(error);
}