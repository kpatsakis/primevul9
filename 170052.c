static void add_warning(Scanner *s, char *error)
{
	s->errors->warning_count++;
	s->errors->warning_messages = timelib_realloc(s->errors->warning_messages, s->errors->warning_count * sizeof(timelib_error_message));
	s->errors->warning_messages[s->errors->warning_count - 1].position = s->tok ? s->tok - s->str : 0;
	s->errors->warning_messages[s->errors->warning_count - 1].character = s->tok ? *s->tok : 0;
	s->errors->warning_messages[s->errors->warning_count - 1].message = timelib_strdup(error);
}