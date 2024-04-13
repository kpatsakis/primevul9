mctxinit(void **target, void *arg) {
	isc_result_t result;
	isc_mem_t *mctx = NULL;

	UNUSED(arg);

	REQUIRE(target != NULL && *target == NULL);

	result = isc_mem_create(0, 0, &mctx);
	if (result != ISC_R_SUCCESS)
		return (result);
	isc_mem_setname(mctx, "zonemgr-pool", NULL);

	*target = mctx;
	return (ISC_R_SUCCESS);
}