static void mark_line(fixed sx, fixed sy, fixed ex, fixed ey, int base_y, int height, int *table, int *index)
{
    int delta;
    int isy = fixed2int(sy + fixed_half);
    int iey = fixed2int(ey + fixed_half);
    int dirn = DIRN_UP;

#ifdef DEBUG_SCAN_CONVERTER
    dlprintf2("Marking line from %d to %d\n", isy, iey);
#endif

    if (isy == iey)
        return;
    if (isy > iey) {
        int t;
        t = isy; isy = iey; iey = t;
        t = sx; sx = ex; ex = t;
        dirn = DIRN_DOWN;
    }
    /* So we now have to mark a line of intersects from (sx,sy) to (ex,ey) */
    iey -= isy;
    ex -= sx;
    isy -= base_y;
#ifdef DEBUG_SCAN_CONVERTER
    dlprintf2("    sy=%d ey=%d\n", isy, iey);
#endif
    if (ex >= 0) {
        int x_inc, n_inc, f;

        /* We want to change sx by ex in iey steps. So each step, we add
         * ex/iey to sx. That's x_inc + n_inc/iey.
         */
       x_inc = ex/iey;
       n_inc = ex-(x_inc*iey);
       f     = iey>>1;
       /* Do a half step to start us off */
       sx += x_inc>>1;
       f  -= n_inc>>1;
       if (f < 0) {
           f += iey;
           sx++;
       }
       if (x_inc & 1) {
           f -= n_inc>>2;
           if (f < 0) {
               f += iey;
               sx++;
           }
       }
       delta = iey;
       do {
           int *row;

           if (isy >= 0 && isy < height) {
               row = &table[index[isy]];
               *row = (*row)+1; /* Increment the count */
               row[*row] = (sx&~1) | dirn;
           }
           isy++;
           sx += x_inc;
           f  -= n_inc;
           if (f < 0) {
               f += iey;
               sx++;
           }
        } while (--delta);
    } else {
        int x_dec, n_dec, f;

        ex = -ex;
        /* We want to change sx by ex in iey steps. So each step, we subtract
         * ex/iey from sx. That's x_dec + n_dec/iey.
         */
        x_dec = ex/iey;
        n_dec = ex-(x_dec*iey);
        f     = iey>>1;
        /* Do a half step to start us off */
        sx -= x_dec>>1;
        f  -= n_dec>>1;
        if (f < 0) {
            f += iey;
            sx--;
        }
        if (x_dec & 1) {
            f -= n_dec>>2;
            if (f < 0) {
                f += iey;
                sx--;
            }
        }
        delta = iey;
        do {
            int *row;

            if (isy >= 0 && isy < height) {
                row = &table[index[isy]];
                (*row)++; /* Increment the count */
                row[*row] = (sx&~1) | dirn;
            }
            isy++;
            sx -= x_dec;
            f  -= n_dec;
            if (f < 0) {
                f += iey;
                sx--;
            }
         } while (--delta);
    }
}