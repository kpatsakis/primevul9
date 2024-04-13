int sc_compare_oid(const struct sc_object_id *oid1, const struct sc_object_id *oid2)
{
	int i;

	if (oid1 == NULL || oid2 == NULL) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	for (i = 0; i < SC_MAX_OBJECT_ID_OCTETS; i++)   {
		if (oid1->value[i] != oid2->value[i])
			return 0;
		if (oid1->value[i] == -1)
			break;
	}

	return 1;
}