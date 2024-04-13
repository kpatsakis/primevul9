static void lru_maintainer_crawler_check(struct crawler_expired_data *cdata, logger *l) {
    int i;
    static rel_time_t next_crawls[MAX_NUMBER_OF_SLAB_CLASSES];
    static rel_time_t next_crawl_wait[MAX_NUMBER_OF_SLAB_CLASSES];
    uint8_t todo[MAX_NUMBER_OF_SLAB_CLASSES];
    memset(todo, 0, sizeof(uint8_t) * MAX_NUMBER_OF_SLAB_CLASSES);
    bool do_run = false;
    if (!cdata->crawl_complete) {
        return;
    }

    for (i = POWER_SMALLEST; i < MAX_NUMBER_OF_SLAB_CLASSES; i++) {
        crawlerstats_t *s = &cdata->crawlerstats[i];
        /* We've not successfully kicked off a crawl yet. */
        if (s->run_complete) {
            pthread_mutex_lock(&cdata->lock);
            int x;
            /* Should we crawl again? */
            uint64_t possible_reclaims = s->seen - s->noexp;
            uint64_t available_reclaims = 0;
            /* Need to think we can free at least 1% of the items before
             * crawling. */
            /* FIXME: Configurable? */
            uint64_t low_watermark = (possible_reclaims / 100) + 1;
            rel_time_t since_run = current_time - s->end_time;
            /* Don't bother if the payoff is too low. */
            for (x = 0; x < 60; x++) {
                available_reclaims += s->histo[x];
                if (available_reclaims > low_watermark) {
                    if (next_crawl_wait[i] < (x * 60)) {
                        next_crawl_wait[i] += 60;
                    } else if (next_crawl_wait[i] >= 60) {
                        next_crawl_wait[i] -= 60;
                    }
                    break;
                }
            }

            if (available_reclaims == 0) {
                next_crawl_wait[i] += 60;
            }

            if (next_crawl_wait[i] > MAX_MAINTCRAWL_WAIT) {
                next_crawl_wait[i] = MAX_MAINTCRAWL_WAIT;
            }

            next_crawls[i] = current_time + next_crawl_wait[i] + 5;
            LOGGER_LOG(l, LOG_SYSEVENTS, LOGGER_CRAWLER_STATUS, NULL, i, (unsigned long long)low_watermark,
                    (unsigned long long)available_reclaims,
                    (unsigned int)since_run,
                    next_crawls[i] - current_time,
                    s->end_time - s->start_time,
                    s->seen,
                    s->reclaimed);
            // Got our calculation, avoid running until next actual run.
            s->run_complete = false;
            pthread_mutex_unlock(&cdata->lock);
        }
        if (current_time > next_crawls[i]) {
            todo[i] = 1;
            do_run = true;
            next_crawls[i] = current_time + 5; // minimum retry wait.
        }
    }
    if (do_run) {
        lru_crawler_start(todo, settings.lru_crawler_tocrawl, CRAWLER_EXPIRED, cdata, NULL, 0);
    }
}