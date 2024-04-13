dns_zonemgr_setsize(dns_zonemgr_t *zmgr, int num_zones) {
	isc_result_t result;
	int ntasks = num_zones / ZONES_PER_TASK;
	int nmctx = num_zones / ZONES_PER_MCTX;
	isc_taskpool_t *pool = NULL;
	isc_pool_t *mctxpool = NULL;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	/*
	 * For anything fewer than 1000 zones we use 10 tasks in
	 * the task pools.  More than that, and we'll scale at one
	 * task per 100 zones.  Similarly, for anything smaller than
	 * 2000 zones we use 2 memory contexts, then scale at 1:1000.
	 */
	if (ntasks < 10)
		ntasks = 10;
	if (nmctx < 2)
		nmctx = 2;

	/* Create or resize the zone task pools. */
	if (zmgr->zonetasks == NULL)
		result = isc_taskpool_create(zmgr->taskmgr, zmgr->mctx,
					     ntasks, 2, &pool);
	else
		result = isc_taskpool_expand(&zmgr->zonetasks, ntasks, &pool);

	if (result == ISC_R_SUCCESS)
		zmgr->zonetasks = pool;

	pool = NULL;
	if (zmgr->loadtasks == NULL)
		result = isc_taskpool_create(zmgr->taskmgr, zmgr->mctx,
					     ntasks, 2, &pool);
	else
		result = isc_taskpool_expand(&zmgr->loadtasks, ntasks, &pool);

	if (result == ISC_R_SUCCESS)
		zmgr->loadtasks = pool;

	/*
	 * We always set all tasks in the zone-load task pool to
	 * privileged.  This prevents other tasks in the system from
	 * running while the server task manager is in privileged
	 * mode.
	 *
	 * NOTE: If we start using task privileges for any other
	 * part of the system than zone tasks, then this will need to be
	 * revisted.  In that case we'd want to turn on privileges for
	 * zone tasks only when we were loading, and turn them off the
	 * rest of the time.  For now, however, it's okay to just
	 * set it and forget it.
	 */
	isc_taskpool_setprivilege(zmgr->loadtasks, true);

	/* Create or resize the zone memory context pool. */
	if (zmgr->mctxpool == NULL)
		result = isc_pool_create(zmgr->mctx, nmctx, mctxfree,
					 mctxinit, NULL, &mctxpool);
	else
		result = isc_pool_expand(&zmgr->mctxpool, nmctx, &mctxpool);

	if (result == ISC_R_SUCCESS)
		zmgr->mctxpool = mctxpool;

	return (result);
}