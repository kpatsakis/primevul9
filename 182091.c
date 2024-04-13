static int process_ref(const char *line, int len, struct ref ***list,
		       unsigned int flags, struct oid_array *extra_have)
{
	struct object_id old_oid;
	const char *name;

	if (parse_oid_hex(line, &old_oid, &name))
		return 0;
	if (*name != ' ')
		return 0;
	name++;

	if (extra_have && !strcmp(name, ".have")) {
		oid_array_append(extra_have, &old_oid);
	} else if (!strcmp(name, "capabilities^{}")) {
		die(_("protocol error: unexpected capabilities^{}"));
	} else if (check_ref(name, flags)) {
		struct ref *ref = alloc_ref(name);
		oidcpy(&ref->old_oid, &old_oid);
		**list = ref;
		*list = &ref->next;
	}
	check_no_capabilities(line, len);
	return 1;
}