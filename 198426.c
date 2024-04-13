int sc_format_oid(struct sc_object_id *oid, const char *in)
{
	int        ii, ret = SC_ERROR_INVALID_ARGUMENTS;
	const char *p;
	char       *q;

	if (oid == NULL || in == NULL)
		return SC_ERROR_INVALID_ARGUMENTS;

	sc_init_oid(oid);

	p = in;
	for (ii=0; ii < SC_MAX_OBJECT_ID_OCTETS; ii++)   {
		oid->value[ii] = strtol(p, &q, 10);
		if (!*q)
			break;

		if (!(q[0] == '.' && isdigit(q[1])))
			goto out;

		p = q + 1;
	}

	if (!sc_valid_oid(oid))
		goto out;

	ret = SC_SUCCESS;
out:
	if (ret)
		sc_init_oid(oid);

	return ret;
}