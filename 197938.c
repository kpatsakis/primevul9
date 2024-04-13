static void worker_thread(void *arg)
{
	worker_t *me = (worker_t *)arg;
	int band_start;

	do
	{
		DEBUG_THREADS(("Worker %d waiting\n", me->num));
		mu_wait_semaphore(&me->start);
		band_start = me->band_start;
		DEBUG_THREADS(("Worker %d woken for band_start %d\n", me->num, me->band_start));
		me->status = RENDER_OK;
		if (band_start >= 0)
			me->status = drawband(me->ctx, NULL, me->list, me->ctm, me->tbounds, &me->cookie, band_start, me->pix, &me->bit);
		DEBUG_THREADS(("Worker %d completed band_start %d (status=%d)\n", me->num, band_start, me->status));
		mu_trigger_semaphore(&me->stop);
	}
	while (band_start >= 0);
}