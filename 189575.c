ns_clientmgr_createclients(ns_clientmgr_t *manager, unsigned int n,
			   ns_interface_t *ifp, bool tcp)
{
	isc_result_t result = ISC_R_SUCCESS;
	unsigned int disp;

	REQUIRE(VALID_MANAGER(manager));
	REQUIRE(n > 0);

	MTRACE("createclients");

	for (disp = 0; disp < n; disp++) {
		result = get_client(manager, ifp, ifp->udpdispatch[disp], tcp);
		if (result != ISC_R_SUCCESS)
			break;
	}

	return (result);
}