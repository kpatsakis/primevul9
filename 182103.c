static int process_shallow(const char *line, int len,
			   struct oid_array *shallow_points)
{
	const char *arg;
	struct object_id old_oid;

	if (!skip_prefix(line, "shallow ", &arg))
		return 0;

	if (get_oid_hex(arg, &old_oid))
		die(_("protocol error: expected shallow sha-1, got '%s'"), arg);
	if (!shallow_points)
		die(_("repository on the other end cannot be shallow"));
	oid_array_append(shallow_points, &old_oid);
	check_no_capabilities(line, len);
	return 1;
}