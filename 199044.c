static void *lru_maintainer_thread(void *arg) {
    int i;
    useconds_t to_sleep = MIN_LRU_MAINTAINER_SLEEP;
    useconds_t last_sleep = MIN_LRU_MAINTAINER_SLEEP;
    rel_time_t last_crawler_check = 0;
    useconds_t next_juggles[MAX_NUMBER_OF_SLAB_CLASSES];
    useconds_t backoff_juggles[MAX_NUMBER_OF_SLAB_CLASSES];
    struct crawler_expired_data cdata;
    memset(&cdata, 0, sizeof(struct crawler_expired_data));
    memset(next_juggles, 0, sizeof(next_juggles));
    pthread_mutex_init(&cdata.lock, NULL);
    cdata.crawl_complete = true; // kick off the crawler.
    logger *l = logger_create();
    if (l == NULL) {
        fprintf(stderr, "Failed to allocate logger for LRU maintainer thread\n");
        abort();
    }

    pthread_mutex_lock(&lru_maintainer_lock);
    if (settings.verbose > 2)
        fprintf(stderr, "Starting LRU maintainer background thread\n");
    while (do_run_lru_maintainer_thread) {
        pthread_mutex_unlock(&lru_maintainer_lock);
        if (to_sleep)
            usleep(to_sleep);
        pthread_mutex_lock(&lru_maintainer_lock);
        /* A sleep of zero counts as a minimum of a 1ms wait */
        last_sleep = to_sleep > 1000 ? to_sleep : 1000;
        to_sleep = MAX_LRU_MAINTAINER_SLEEP;

        STATS_LOCK();
        stats.lru_maintainer_juggles++;
        STATS_UNLOCK();

        /* Each slab class gets its own sleep to avoid hammering locks */
        for (i = POWER_SMALLEST; i < MAX_NUMBER_OF_SLAB_CLASSES; i++) {
            next_juggles[i] = next_juggles[i] > last_sleep ? next_juggles[i] - last_sleep : 0;

            // Sleep the thread just for the minimum amount (or not at all)
            if (next_juggles[i] < to_sleep) {
                to_sleep = next_juggles[i];
            }
            if (next_juggles[i] > 0)
                continue;

            int did_moves = lru_maintainer_juggle(i);
            if (did_moves == 0) {
                if (backoff_juggles[i] < MAX_LRU_MAINTAINER_SLEEP)
                    backoff_juggles[i] += 1000;
            } else if (backoff_juggles[i] > 0) {
                backoff_juggles[i] /= 2;
                if (backoff_juggles[i] < MIN_LRU_MAINTAINER_SLEEP) {
                    backoff_juggles[i] = 0;
                }
            }
            next_juggles[i] = backoff_juggles[i];
        }

        /* Minimize the sleep if we had async LRU bumps to process */
        if (settings.lru_segmented && lru_maintainer_bumps() && to_sleep > 1000) {
            to_sleep = 1000;
        }

        /* Once per second at most */
        if (settings.lru_crawler && last_crawler_check != current_time) {
            lru_maintainer_crawler_check(&cdata, l);
            last_crawler_check = current_time;
        }
    }
    pthread_mutex_unlock(&lru_maintainer_lock);
    if (settings.verbose > 2)
        fprintf(stderr, "LRU maintainer thread stopping\n");

    return NULL;
}