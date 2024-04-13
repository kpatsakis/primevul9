gx_fill_edgebuffer_tr(gx_device       * pdev,
                const gx_device_color * pdevc,
                      gx_edgebuffer   * edgebuffer,
                      int               log_op)
{
    int i, j, code;

    for (i=0; i < edgebuffer->height; ) {
        int *row    = &edgebuffer->table[edgebuffer->index[i]];
        int  rowlen = *row++;
        int *row2;
        int *rowptr;
        int *row2ptr;

        /* See how many scanlines match i */
        for (j = i+1; j < edgebuffer->height; j++) {
            int row2len;

            row2    = &edgebuffer->table[edgebuffer->index[j]];
            row2len = *row2++;
            row2ptr = row2;
            rowptr  = row;

            if (rowlen != row2len)
                break;
            while (row2len > 0) {
                if ((rowptr[1]&~1) != (row2ptr[1]&~1))
                    goto rowdifferent;
                rowptr  += 2;
                row2ptr += 2;
                row2len -= 2;
            }
        }
rowdifferent:{}

        /* So j is the first scanline that doesn't match i */

        if (j == i+1) {
            while (rowlen > 0) {
                int left, right;

                left  = row[0];
                right = row[2];
                row += 4;
                rowlen -= 2;

                left  = fixed2int(left + fixed_half);
                right = fixed2int(right + fixed_half);
                right -= left;
                if (right > 0) {
                    if (log_op < 0)
                        code = dev_proc(pdev, fill_rectangle)(pdev, left, edgebuffer->base+i, right, 1, pdevc->colors.pure);
                    else
                        code = gx_fill_rectangle_device_rop(left, edgebuffer->base+i, right, 1, pdevc, pdev, (gs_logical_operation_t)log_op);
                    if (code < 0)
                        return code;
                }
            }
        } else {
            gs_fixed_edge le;
            gs_fixed_edge re;
            le.start.y = re.start.y = int2fixed(edgebuffer->base+i);
            le.end.y   = re.end.y   = int2fixed(edgebuffer->base+j)-1;
            row2    = &edgebuffer->table[edgebuffer->index[j-1]+1];
            while (rowlen > 0) {
                le.start.x = row[0] - fixed_half;
                re.start.x = row[2] + fixed_half;
                le.end.x   = row2[0] - fixed_half;
                re.end.x   = row2[2] + fixed_half;
                row += 4;
                row2 += 4;
                rowlen -= 2;

                assert(le.start.x >= 0);
                assert(le.end.x >= 0);
                assert(re.start.x >= le.start.x);
                assert(re.end.x >= le.end.x);

                code = dev_proc(pdev, fill_trapezoid)(
                                pdev,
                                &le,
                                &re,
                                le.start.y,
                                le.end.y,
                                0, /* bool swap_axes */
                                pdevc, /*const gx_drawing_color *pdcolor */
                                log_op);
                if (code < 0)
                    return code;
            }
        }

        i = j;
    }
    return 0;
}