gx_filter_edgebuffer_tr(gx_device       * pdev,
                        gx_edgebuffer   * edgebuffer,
                        int               rule)
{
    int i;

#ifdef DEBUG_SCAN_CONVERTER
    gx_edgebuffer_print(edgebuffer);
#endif

    for (i=0; i < edgebuffer->height; i++) {
        int *row      = &edgebuffer->table[edgebuffer->index[i]];
        int  rowlen   = *row++;
        int *rowstart = row;
        int *rowout   = row;

        while (rowlen > 0) {
            int left, lid, right, rid;

            if (rule == gx_rule_even_odd) {
                /* Even Odd */
                left  = *row++;
                lid   = *row++;
                right = *row++;
                rid   = *row++;
                rowlen -= 2;
            } else {
                /* Non-Zero */
                int w;

                left = *row++;
                lid  = *row++;
                w = ((lid&1)-1) | 1;
                rowlen--;
                do {
                    right = *row++;
                    rid   = *row++;
                    rowlen--;
                    w += ((rid&1)-1) | 1;
                } while (w != 0);
            }

            if (right > left) {
                *rowout++ = left;
                *rowout++ = lid;
                *rowout++ = right;
                *rowout++ = rid;
            }
        }
        rowstart[-1] = (rowout-rowstart)>>1;
    }
    return 0;
}