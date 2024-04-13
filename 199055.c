static int lru_maintainer_juggle(const int slabs_clsid) {
    int i;
    int did_moves = 0;
    bool mem_limit_reached = false;
    uint64_t total_bytes = 0;
    unsigned int chunks_perslab = 0;
    unsigned int chunks_free = 0;
    /* TODO: if free_chunks below high watermark, increase aggressiveness */
    chunks_free = slabs_available_chunks(slabs_clsid, &mem_limit_reached,
            &total_bytes, &chunks_perslab);
    if (settings.temp_lru) {
        /* Only looking for reclaims. Run before we size the LRU. */
        for (i = 0; i < 500; i++) {
            if (lru_pull_tail(slabs_clsid, TEMP_LRU, 0, 0, 0) <= 0) {
                break;
            } else {
                did_moves++;
            }
        }
        total_bytes -= temp_lru_size(slabs_clsid);
    }

    /* If slab automove is enabled on any level, and we have more than 2 pages
     * worth of chunks free in this class, ask (gently) to reassign a page
     * from this class back into the global pool (0)
     */
    if (settings.slab_automove > 0 && chunks_free > (chunks_perslab * 2.5)) {
        slabs_reassign(slabs_clsid, SLAB_GLOBAL_PAGE_POOL);
    }

    rel_time_t cold_age = 0;
    rel_time_t hot_age = 0;
    /* If LRU is in flat mode, force items to drain into COLD via max age */
    if (settings.lru_segmented) {
        hot_age = settings.hot_max_age;
        pthread_mutex_lock(&lru_locks[slabs_clsid|COLD_LRU]);
        if (tails[slabs_clsid|COLD_LRU]) {
            cold_age = current_time - tails[slabs_clsid|COLD_LRU]->time;
        }
        pthread_mutex_unlock(&lru_locks[slabs_clsid|COLD_LRU]);
    }

    /* Juggle HOT/WARM up to N times */
    for (i = 0; i < 500; i++) {
        int do_more = 0;
        if (lru_pull_tail(slabs_clsid, HOT_LRU, total_bytes, LRU_PULL_CRAWL_BLOCKS, hot_age) ||
            lru_pull_tail(slabs_clsid, WARM_LRU, total_bytes, LRU_PULL_CRAWL_BLOCKS, cold_age * settings.warm_max_factor)) {
            do_more++;
        }
        if (settings.lru_segmented) {
            do_more += lru_pull_tail(slabs_clsid, COLD_LRU, total_bytes, LRU_PULL_CRAWL_BLOCKS, 0);
        }
        if (do_more == 0)
            break;
        did_moves++;
    }
    return did_moves;
}