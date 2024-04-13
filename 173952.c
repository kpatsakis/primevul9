static void find_model_params(Encoder *encoder,
                              const int bpc,
                              unsigned int *ncounters,
                              unsigned int *levels,
                              unsigned int *n_buckets_ptrs,
                              unsigned int *repfirst,
                              unsigned int *firstsize,
                              unsigned int *repnext,
                              unsigned int *mulsize,
                              unsigned int *nbuckets)
{
    unsigned int bsize;              /* bucket size */
    unsigned int bstart, bend = 0;   /* bucket start and end, range : 0 to levels-1*/
    unsigned int repcntr;            /* helper */

    /* The only valid values are 1, 3 and 5.
       0, 2 and 4 are obsolete and the rest of the
       values are considered out of the range. */
    SPICE_VERIFY(DEFevol == 1 || DEFevol == 3 || DEFevol == 5);
    spice_assert(bpc <= 8 && bpc > 0);

    *ncounters = 8;

    *levels = 0x1 << bpc;

    *n_buckets_ptrs = 0;  /* ==0 means: not set yet */

    switch (DEFevol) {   /* set repfirst firstsize repnext mulsize */
    case 1: /* buckets contain following numbers of contexts: 1 1 1 2 2 4 4 8 8 ... */
        *repfirst = 3;
        *firstsize = 1;
        *repnext = 2;
        *mulsize = 2;
        break;
    case 3: /* 1 2 4 8 16 32 64 ... */
        *repfirst = 1;
        *firstsize = 1;
        *repnext = 1;
        *mulsize = 2;
        break;
    case 5:                     /* 1 4 16 64 256 1024 4096 16384 65536 */
        *repfirst = 1;
        *firstsize = 1;
        *repnext = 1;
        *mulsize = 4;
        break;
    default:
        encoder->usr->error(encoder->usr, "findmodelparams(): DEFevol out of range!!!\n");
        return;
    }

    *nbuckets = 0;
    repcntr = *repfirst + 1;    /* first bucket */
    bsize = *firstsize;

    do { /* other buckets */
        if (*nbuckets) {         /* bucket start */
            bstart = bend + 1;
        } else {
            bstart = 0;
        }

        if (!--repcntr) {         /* bucket size */
            repcntr = *repnext;
            bsize *= *mulsize;
        }

        bend = bstart + bsize - 1;  /* bucket end */
        if (bend + bsize >= *levels) {  /* if following bucked was bigger than current one */
            bend = *levels - 1;     /* concatenate them */
        }

        if (!*n_buckets_ptrs) {           /* array size not set yet? */
            *n_buckets_ptrs = *levels;
 #if 0
            if (bend == *levels - 1) {   /* this bucket is last - all in the first array */
                *n_buckets_ptrs = *levels;
            } else if (bsize >= 256) { /* this bucket is allowed to reside in the 2nd table */
                b_lo_ptrs = bstart;
                spice_assert(bstart);     /* previous bucket exists */
            }
 #endif
        }

        (*nbuckets)++;
    } while (bend < *levels - 1);
}