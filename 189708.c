static void modsecurity_persist_data(modsec_rec *msr) {
    const apr_array_header_t *arr;
    apr_table_entry_t *te;
    apr_time_t time_before, time_after;
    int i;

    time_before = apr_time_now();

    /* Collections, store & remove stale. */
    arr = apr_table_elts(msr->collections);
    te = (apr_table_entry_t *)arr->elts;
    for (i = 0; i < arr->nelts; i++) {
        apr_table_t *col = (apr_table_t *)te[i].val;

        /* Only store those collections that changed. */
        if (apr_table_get(msr->collections_dirty, te[i].key)) {
            collection_store(msr, col);
        }
    }

    time_after = apr_time_now();

    msr->time_storage_write += time_after - time_before;

    if (msr->txcfg->debuglog_level >= 4) {
        msr_log(msr, 4, "Recording persistent data took %" APR_TIME_T_FMT
            " microseconds.", msr->time_gc);
    }

    /* Remove stale collections. */
    srand(time(NULL));

    if (rand() < RAND_MAX/100) {
        arr = apr_table_elts(msr->collections);
        te = (apr_table_entry_t *)arr->elts;
        for (i = 0; i < arr->nelts; i++) {
            collections_remove_stale(msr, te[i].key);
        }

        msr->time_gc = apr_time_now() - time_after;

        if (msr->txcfg->debuglog_level >= 4) {
            msr_log(msr, 4, "Garbage collection took %" APR_TIME_T_FMT
                " microseconds.", msr->time_gc);
        }
    }
}