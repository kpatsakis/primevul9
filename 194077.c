static bool elevator_match(const struct elevator_type *e, const char *name)
{
	if (!strcmp(e->elevator_name, name))
		return true;
	if (e->elevator_alias && !strcmp(e->elevator_alias, name))
		return true;

	return false;
}