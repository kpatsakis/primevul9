static int init_channel(Encoder *encoder, Channel *channel)
{
    unsigned int ncounters;
    unsigned int levels;
    unsigned int rep_first;
    unsigned int first_size;
    unsigned int rep_next;
    unsigned int mul_size;
    unsigned int n_buckets;
    unsigned int n_buckets_ptrs;

    channel->correlate_row_width = 0;
    channel->correlate_row = NULL;

    find_model_params(encoder, 8, &ncounters, &levels, &n_buckets_ptrs, &rep_first,
                      &first_size, &rep_next, &mul_size, &n_buckets);
    encoder->n_buckets_8bpc = n_buckets;
    if (!init_model_structures(encoder, &channel->family_stat_8bpc, rep_first, first_size,
                               rep_next, mul_size, levels, ncounters, n_buckets_ptrs,
                               n_buckets)) {
        return FALSE;
    }

    find_model_params(encoder, 5, &ncounters, &levels, &n_buckets_ptrs, &rep_first,
                      &first_size, &rep_next, &mul_size, &n_buckets);
    encoder->n_buckets_5bpc = n_buckets;
    if (!init_model_structures(encoder, &channel->family_stat_5bpc, rep_first, first_size,
                               rep_next, mul_size, levels, ncounters, n_buckets_ptrs,
                               n_buckets)) {
        free_family_stat(encoder->usr, &channel->family_stat_8bpc);
        return FALSE;
    }

    return TRUE;
}