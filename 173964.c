static int encoder_reset_channels(Encoder *encoder, int channels, int width, int bpc)
{
    int i;

    for (i = 0; i < channels; i++) {
        s_bucket *bucket;
        s_bucket *end_bucket;

        if (encoder->channels[i].correlate_row_width < width) {
            encoder->channels[i].correlate_row_width = 0;
            if (encoder->channels[i].correlate_row) {
                encoder->usr->free(encoder->usr, encoder->channels[i].correlate_row - 1);
            }
            if (!(encoder->channels[i].correlate_row = (BYTE *)encoder->usr->malloc(encoder->usr,
                                                                                    width + 1))) {
                return FALSE;
            }
            encoder->channels[i].correlate_row++;
            encoder->channels[i].correlate_row_width = width;
        }

        if (bpc == 8) {
            MEMCLEAR(encoder->channels[i].family_stat_8bpc.counters,
                     encoder->n_buckets_8bpc * sizeof(COUNTER) * MAXNUMCODES);
            bucket = encoder->channels[i].family_stat_8bpc.buckets_buf;
            end_bucket = bucket + encoder->n_buckets_8bpc;
            for (; bucket < end_bucket; bucket++) {
                bucket->bestcode = /*BPC*/ 8 - 1;
            }
            encoder->channels[i]._buckets_ptrs = encoder->channels[i].family_stat_8bpc.buckets_ptrs;
        } else if (bpc == 5) {
            MEMCLEAR(encoder->channels[i].family_stat_5bpc.counters,
                     encoder->n_buckets_5bpc * sizeof(COUNTER) * MAXNUMCODES);
            bucket = encoder->channels[i].family_stat_5bpc.buckets_buf;
            end_bucket = bucket + encoder->n_buckets_5bpc;
            for (; bucket < end_bucket; bucket++) {
                bucket->bestcode = /*BPC*/ 5 - 1;
            }
            encoder->channels[i]._buckets_ptrs = encoder->channels[i].family_stat_5bpc.buckets_ptrs;
        } else {
            encoder->usr->warn(encoder->usr, "%s: bad bpc %d\n", __FUNCTION__, bpc);
            return FALSE;
        }

        encoder->channels[i].state.waitcnt = 0;
        encoder->channels[i].state.tabrand_seed = stabrand();
        encoder->channels[i].state.wmidx = DEFwmistart;
        encoder->channels[i].state.wmileft = DEFwminext;
        set_wm_trigger(&encoder->channels[i].state);

        encoder_init_rle(&encoder->channels[i].state);
    }
    return TRUE;
}