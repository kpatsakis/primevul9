gx_filter_edgebuffer(gx_device       * pdev,
                     gx_edgebuffer   * edgebuffer,
                     int               rule)
{
    int i;

#ifdef DEBUG_SCAN_CONVERTER
    gx_edgebuffer_print(edgebuffer);
#endif

    for (i=0; i < edgebuffer->height; i++) {
        int *row      = &edgebuffer->table[edgebuffer->index[i]];
        int *rowstart = row;
        int  rowlen   = *row++;
        int *rowout   = row;

        while (rowlen > 0)
        {
            int left, right;

            if (rule == gx_rule_even_odd) {
                /* Even Odd */
                left  = (*row++)&~1;
                right = (*row++)&~1;
                rowlen -= 2;
            } else {
                /* Non-Zero */
                int w;

                left = *row++;
                w = ((left&1)-1) | (left&1);
                rowlen--;
                do {
                    right  = *row++;
                    rowlen--;
                    w += ((right&1)-1) | (right&1);
                } while (w != 0);
                left &= ~1;
                right &= ~1;
            }

            if (right > left) {
                *rowout++ = left;
                *rowout++ = right;
            }
        }
        *rowstart = (rowout-rowstart)-1;
    }
    return 0;
}