static void add_pbf_error(Scanner *s, char *error, char *sptr, char *cptr)
{
	s->errors->error_count++;
	s->errors->error_messages = timelib_realloc(s->errors->error_messages, s->errors->error_count * sizeof(timelib_error_message));
	s->errors->error_messages[s->errors->error_count - 1].position = cptr - sptr;
	s->errors->error_messages[s->errors->error_count - 1].character = *cptr;
	s->errors->error_messages[s->errors->error_count - 1].message = timelib_strdup(error);
}