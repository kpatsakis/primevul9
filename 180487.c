gx_filter_edgebuffer_app(gx_device       * pdev,
                         gx_edgebuffer   * edgebuffer,
                         int               rule)
{
    int i;

#ifdef DEBUG_SCAN_CONVERTER
    gx_edgebuffer_print_app(edgebuffer);
#endif

    for (i=0; i < edgebuffer->height; i++) {
        int *row      = &edgebuffer->table[edgebuffer->index[i]];
        int  rowlen   = *row++;
        int *rowstart = row;
        int *rowout   = row;
        int  ll, lr, rl, rr, wind, marked_to;

        /* Avoid double setting pixels, by keeping where we have marked to. */
        marked_to = 0;
        while (rowlen > 0) {
            if (rule == gx_rule_even_odd) {
                /* Even Odd */
                ll = (*row++)&~1;
                lr = (*row++);
                rowlen--;
                wind = 1;

                /* We will fill solidly from ll to at least lr, possibly further */
                assert(rowlen > 0);
                do {
                    rl = (*row++)&~1;
                    rr = (*row++);
                    rowlen--;
                    if (rr > lr)
                        lr = rr;
                    wind ^= 1;
                    if (wind == 0)
                        break;
                } while (rowlen > 0);
            } else {
                /* Non-Zero */
                int w;

                ll = *row++;
                lr = *row++;
                wind = -(ll&1) | 1;
                ll &= ~1;
                rowlen--;

                assert(rowlen > 0);
                do {
                    rl = (*row++);
                    rr = (*row++);
                    w = -(rl&1) | 1;
                    rl &= ~1;
                    rowlen--;
                    if (rr > lr)
                        lr = rr;
                    wind += w;
                    if (wind == 0)
                        break;
                } while (rowlen > 0);
            }

            if (marked_to > ll) {
                if (rowout == rowstart)
                    ll = marked_to;
                else {
                    rowout -= 2;
                    ll = *rowout;
                }
            }

            if (lr > ll) {
                *rowout++ = ll;
                *rowout++ = lr;
                marked_to = ll;
            }
        }
        rowstart[-1] = rowout - rowstart;
    }
    return 0;
}