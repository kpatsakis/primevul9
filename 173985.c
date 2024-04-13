static int init_model_structures(Encoder *encoder, FamilyStat *family_stat,
                                 unsigned int rep_first, unsigned int first_size,
                                 unsigned int rep_next, unsigned int mul_size,
                                 unsigned int levels, unsigned int ncounters,
                                 unsigned int n_buckets_ptrs, unsigned int n_buckets)
{
    family_stat->buckets_ptrs = (s_bucket **)encoder->usr->malloc(encoder->usr,
                                                                  n_buckets_ptrs *
                                                                  sizeof(s_bucket *));
    if (!family_stat->buckets_ptrs) {
        return FALSE;
    }

    family_stat->counters = (COUNTER *)encoder->usr->malloc(encoder->usr,
                                                            n_buckets * sizeof(COUNTER) *
                                                            MAXNUMCODES);
    if (!family_stat->counters) {
        goto error_1;
    }

    family_stat->buckets_buf = (s_bucket *)encoder->usr->malloc(encoder->usr,
                                                                n_buckets * sizeof(s_bucket));
    if (!family_stat->buckets_buf) {
        goto error_2;
    }

    fill_model_structures(encoder, family_stat, rep_first, first_size, rep_next, mul_size, levels,
                          ncounters, n_buckets, n_buckets_ptrs);

    return TRUE;

error_2:
    encoder->usr->free(encoder->usr, family_stat->counters);

error_1:
    encoder->usr->free(encoder->usr, family_stat->buckets_ptrs);

    return FALSE;
}