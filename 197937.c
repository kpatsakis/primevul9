static void bgprint_worker(void *arg)
{
	fz_cookie cookie = { 0 };
	int pagenum;

	(void)arg;

	do
	{
		DEBUG_THREADS(("BGPrint waiting\n"));
		mu_wait_semaphore(&bgprint.start);
		pagenum = bgprint.pagenum;
		DEBUG_THREADS(("BGPrint woken for pagenum %d\n", pagenum));
		if (pagenum >= 0)
		{
			int start = gettime();
			memset(&cookie, 0, sizeof(cookie));
			bgprint.status = try_render_page(bgprint.ctx, pagenum, &cookie, start, bgprint.interptime, bgprint.filename, 1, bgprint.solo, &bgprint.render);
		}
		DEBUG_THREADS(("BGPrint completed page %d\n", pagenum));
		mu_trigger_semaphore(&bgprint.stop);
	}
	while (pagenum >= 0);
}