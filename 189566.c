ns_clientmgr_create(isc_mem_t *mctx, isc_taskmgr_t *taskmgr,
		    isc_timermgr_t *timermgr, ns_clientmgr_t **managerp)
{
	ns_clientmgr_t *manager;
	isc_result_t result;
#if NMCTXS > 0
	int i;
#endif

	manager = isc_mem_get(mctx, sizeof(*manager));
	if (manager == NULL)
		return (ISC_R_NOMEMORY);

	result = isc_mutex_init(&manager->lock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_manager;

	result = isc_mutex_init(&manager->listlock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_lock;

	result = isc_mutex_init(&manager->reclock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_listlock;

	manager->mctx = mctx;
	manager->taskmgr = taskmgr;
	manager->timermgr = timermgr;
	manager->exiting = false;
	ISC_LIST_INIT(manager->clients);
	ISC_LIST_INIT(manager->recursing);
	ISC_QUEUE_INIT(manager->inactive, ilink);
#if NMCTXS > 0
	manager->nextmctx = 0;
	for (i = 0; i < NMCTXS; i++)
		manager->mctxpool[i] = NULL; /* will be created on-demand */
#endif
	manager->magic = MANAGER_MAGIC;

	MTRACE("create");

	*managerp = manager;

	return (ISC_R_SUCCESS);

 cleanup_listlock:
	(void) isc_mutex_destroy(&manager->listlock);

 cleanup_lock:
	(void) isc_mutex_destroy(&manager->lock);

 cleanup_manager:
	isc_mem_put(manager->mctx, manager, sizeof(*manager));

	return (result);
}