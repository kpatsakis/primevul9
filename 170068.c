static void add_error(Scanner *s, char *error)
{
	s->errors->error_count++;
	s->errors->error_messages = timelib_realloc(s->errors->error_messages, s->errors->error_count * sizeof(timelib_error_message));
	s->errors->error_messages[s->errors->error_count - 1].position = s->tok ? s->tok - s->str : 0;
	s->errors->error_messages[s->errors->error_count - 1].character = s->tok ? *s->tok : 0;
	s->errors->error_messages[s->errors->error_count - 1].message = timelib_strdup(error);
}