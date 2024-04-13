int sc_valid_oid(const struct sc_object_id *oid)
{
	int ii;

	if (!oid)
		return 0;
	if (oid->value[0] == -1 || oid->value[1] == -1)
		return 0;
	if (oid->value[0] > 2 || oid->value[1] > 39)
		return 0;
	for (ii=0;ii<SC_MAX_OBJECT_ID_OCTETS;ii++)
		if (oid->value[ii])
			break;
	if (ii==SC_MAX_OBJECT_ID_OCTETS)
		return 0;
	return 1;
}