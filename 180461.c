gx_filter_edgebuffer_tr_app(gx_device       * pdev,
                            gx_edgebuffer   * edgebuffer,
                            int               rule)
{
    int i;
    int marked_id = 0;

#ifdef DEBUG_SCAN_CONVERTER
    gx_edgebuffer_print_tr_app(edgebuffer);
#endif

    for (i=0; i < edgebuffer->height; i++) {
        int *row      = &edgebuffer->table[edgebuffer->index[i]];
        int  rowlen   = *row++;
        int *rowstart = row;
        int *rowout   = row;
        int  ll, llid, lr, lrid, rlid, rr, rrid, wind, marked_to;

        /* Avoid double setting pixels, by keeping where we have marked to. */
        marked_to = 0;
        while (rowlen > 0) {
            if (rule == gx_rule_even_odd) {
                /* Even Odd */
                ll   = *row++;
                llid = (*row++)>>1;
                lr   = *row++;
                lrid = *row++;
                rowlen--;
                wind = 1;

                /* We will fill solidly from ll to at least lr, possibly further */
                assert(rowlen > 0);
                do {
                    (void)row++; /* rl not needed here */
                    rlid = *row++>>1;
                    rr   = *row++;
                    rrid = *row++;
                    rowlen--;
                    if (rr > lr) {
                        lr   = rr;
                        lrid = rrid;
                    }
                    wind ^= 1;
                    if (wind == 0)
                        break;
                } while (rowlen > 0);
            } else {
                /* Non-Zero */
                int w;

                ll   = *row++;
                llid = *row++;
                lr   = *row++;
                lrid = *row++;
                wind = -(llid&1) | 1;
                llid >>= 1;
                rowlen--;

                assert(rowlen > 0);
                do {
                    (void)row++; /* rl not needed */
                    rlid = *row++;
                    rr   = *row++;
                    rrid = *row++;
                    w = -(rlid&1) | 1;
                    rlid >>= 1;
                    rowlen--;
                    if (rr > lr) {
                        lr   = rr;
                        lrid = rrid;
                    }
                    wind += w;
                    if (wind == 0)
                        break;
                } while (rowlen > 0);
            }

            if (lr < 0)
                continue;

            if (marked_to > ll) {
                if (rowout == rowstart) {
                    ll   = marked_to;
                    llid = --marked_id;
                } else {
                    rowout -= 4;
                    ll   = rowout[0];
                    llid = rowout[1];
                }
            }

            if (lr > ll) {
                *rowout++ = ll;
                *rowout++ = llid;
                *rowout++ = lr;
                *rowout++ = lrid;
                marked_to = ll;
            }
        }
        rowstart[-1] = (rowout - rowstart)>>2;
    }
    return 0;
}