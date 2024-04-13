static int process_ref_v2(const char *line, struct ref ***list)
{
	int ret = 1;
	int i = 0;
	struct object_id old_oid;
	struct ref *ref;
	struct string_list line_sections = STRING_LIST_INIT_DUP;
	const char *end;

	/*
	 * Ref lines have a number of fields which are space deliminated.  The
	 * first field is the OID of the ref.  The second field is the ref
	 * name.  Subsequent fields (symref-target and peeled) are optional and
	 * don't have a particular order.
	 */
	if (string_list_split(&line_sections, line, ' ', -1) < 2) {
		ret = 0;
		goto out;
	}

	if (parse_oid_hex(line_sections.items[i++].string, &old_oid, &end) ||
	    *end) {
		ret = 0;
		goto out;
	}

	ref = alloc_ref(line_sections.items[i++].string);

	oidcpy(&ref->old_oid, &old_oid);
	**list = ref;
	*list = &ref->next;

	for (; i < line_sections.nr; i++) {
		const char *arg = line_sections.items[i].string;
		if (skip_prefix(arg, "symref-target:", &arg))
			ref->symref = xstrdup(arg);

		if (skip_prefix(arg, "peeled:", &arg)) {
			struct object_id peeled_oid;
			char *peeled_name;
			struct ref *peeled;
			if (parse_oid_hex(arg, &peeled_oid, &end) || *end) {
				ret = 0;
				goto out;
			}

			peeled_name = xstrfmt("%s^{}", ref->name);
			peeled = alloc_ref(peeled_name);

			oidcpy(&peeled->old_oid, &peeled_oid);
			**list = peeled;
			*list = &peeled->next;

			free(peeled_name);
		}
	}

out:
	string_list_clear(&line_sections, 0);
	return ret;
}