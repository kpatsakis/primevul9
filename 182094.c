static int process_dummy_ref(const char *line)
{
	struct object_id oid;
	const char *name;

	if (parse_oid_hex(line, &oid, &name))
		return 0;
	if (*name != ' ')
		return 0;
	name++;

	return oideq(&null_oid, &oid) && !strcmp(name, "capabilities^{}");
}