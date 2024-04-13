void sc_init_oid(struct sc_object_id *oid)
{
	int ii;

	if (!oid)
		return;
	for (ii=0; ii<SC_MAX_OBJECT_ID_OCTETS; ii++)
		oid->value[ii] = -1;
}