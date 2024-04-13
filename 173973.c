static void fill_model_structures(SPICE_GNUC_UNUSED Encoder *encoder, FamilyStat *family_stat,
                                  unsigned int rep_first, unsigned int first_size,
                                  unsigned int rep_next, unsigned int mul_size,
                                  unsigned int levels, unsigned int ncounters,
                                  unsigned int nbuckets, unsigned int n_buckets_ptrs)
{
    unsigned int
    bsize,
    bstart,
    bend = 0,
    repcntr,
    bnumber;

    COUNTER * free_counter = family_stat->counters;/* first free location in the array of
                                                      counters */

    bnumber = 0;

    repcntr = rep_first + 1;    /* first bucket */
    bsize = first_size;

    do { /* others */
        if (bnumber) {
            bstart = bend + 1;
        } else {
            bstart = 0;
        }

        if (!--repcntr) {
            repcntr = rep_next;
            bsize *= mul_size;
        }

        bend = bstart + bsize - 1;
        if (bend + bsize >= levels) {
            bend = levels - 1;
        }

        family_stat->buckets_buf[bnumber].pcounters = free_counter;
        free_counter += ncounters;

        spice_assert(bstart < n_buckets_ptrs);
        {
            unsigned int i;
            spice_assert(bend < n_buckets_ptrs);
            for (i = bstart; i <= bend; i++) {
                family_stat->buckets_ptrs[i] = family_stat->buckets_buf + bnumber;
            }
        }

        bnumber++;
    } while (bend < levels - 1);

    spice_assert(free_counter - family_stat->counters == (ptrdiff_t)(nbuckets * ncounters));
}