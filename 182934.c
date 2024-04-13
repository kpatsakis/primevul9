dns_zonemgr_create(isc_mem_t *mctx, isc_taskmgr_t *taskmgr,
		   isc_timermgr_t *timermgr, isc_socketmgr_t *socketmgr,
		   dns_zonemgr_t **zmgrp)
{
	dns_zonemgr_t *zmgr;
	isc_result_t result;

	zmgr = isc_mem_get(mctx, sizeof(*zmgr));
	if (zmgr == NULL)
		return (ISC_R_NOMEMORY);
	zmgr->mctx = NULL;
	zmgr->refs = 1;
	isc_mem_attach(mctx, &zmgr->mctx);
	zmgr->taskmgr = taskmgr;
	zmgr->timermgr = timermgr;
	zmgr->socketmgr = socketmgr;
	zmgr->zonetasks = NULL;
	zmgr->loadtasks = NULL;
	zmgr->mctxpool = NULL;
	zmgr->task = NULL;
	zmgr->notifyrl = NULL;
	zmgr->refreshrl = NULL;
	zmgr->startupnotifyrl = NULL;
	zmgr->startuprefreshrl = NULL;
	ISC_LIST_INIT(zmgr->zones);
	ISC_LIST_INIT(zmgr->waiting_for_xfrin);
	ISC_LIST_INIT(zmgr->xfrin_in_progress);
	memset(zmgr->unreachable, 0, sizeof(zmgr->unreachable));
	result = isc_rwlock_init(&zmgr->rwlock, 0, 0);
	if (result != ISC_R_SUCCESS)
		goto free_mem;

	zmgr->transfersin = 10;
	zmgr->transfersperns = 2;

	/* Unreachable lock. */
	result = isc_rwlock_init(&zmgr->urlock, 0, 0);
	if (result != ISC_R_SUCCESS)
		goto free_rwlock;

	/* Create a single task for queueing of SOA queries. */
	result = isc_task_create(taskmgr, 1, &zmgr->task);
	if (result != ISC_R_SUCCESS)
		goto free_urlock;

	isc_task_setname(zmgr->task, "zmgr", zmgr);
	result = isc_ratelimiter_create(mctx, timermgr, zmgr->task,
					&zmgr->notifyrl);
	if (result != ISC_R_SUCCESS)
		goto free_task;

	result = isc_ratelimiter_create(mctx, timermgr, zmgr->task,
					&zmgr->refreshrl);
	if (result != ISC_R_SUCCESS)
		goto free_notifyrl;

	result = isc_ratelimiter_create(mctx, timermgr, zmgr->task,
					&zmgr->startupnotifyrl);
	if (result != ISC_R_SUCCESS)
		goto free_refreshrl;

	result = isc_ratelimiter_create(mctx, timermgr, zmgr->task,
					&zmgr->startuprefreshrl);
	if (result != ISC_R_SUCCESS)
		goto free_startupnotifyrl;

	/* default to 20 refresh queries / notifies per second. */
	setrl(zmgr->notifyrl, &zmgr->notifyrate, 20);
	setrl(zmgr->startupnotifyrl, &zmgr->startupnotifyrate, 20);
	setrl(zmgr->refreshrl, &zmgr->serialqueryrate, 20);
	setrl(zmgr->startuprefreshrl, &zmgr->startupserialqueryrate, 20);
	isc_ratelimiter_setpushpop(zmgr->startupnotifyrl, true);
	isc_ratelimiter_setpushpop(zmgr->startuprefreshrl, true);

	zmgr->iolimit = 1;
	zmgr->ioactive = 0;
	ISC_LIST_INIT(zmgr->high);
	ISC_LIST_INIT(zmgr->low);

	isc_mutex_init(&zmgr->iolock);

	zmgr->magic = ZONEMGR_MAGIC;

	*zmgrp = zmgr;
	return (ISC_R_SUCCESS);

#if 0
 free_iolock:
	isc_mutex_destroy(&zmgr->iolock);
#endif
 free_startupnotifyrl:
	isc_ratelimiter_detach(&zmgr->startupnotifyrl);
 free_refreshrl:
	isc_ratelimiter_detach(&zmgr->refreshrl);
 free_notifyrl:
	isc_ratelimiter_detach(&zmgr->notifyrl);
 free_task:
	isc_task_detach(&zmgr->task);
 free_urlock:
	isc_rwlock_destroy(&zmgr->urlock);
 free_rwlock:
	isc_rwlock_destroy(&zmgr->rwlock);
 free_mem:
	isc_mem_put(zmgr->mctx, zmgr, sizeof(*zmgr));
	isc_mem_detach(&mctx);
	return (result);
}