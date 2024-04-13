gx_fill_edgebuffer(gx_device       * pdev,
             const gx_device_color * pdevc,
                   gx_edgebuffer   * edgebuffer,
                   int               log_op)
{
    int i, code;

    for (i=0; i < edgebuffer->height; i++) {
        int *row    = &edgebuffer->table[edgebuffer->index[i]];
        int  rowlen = *row++;

        while (rowlen > 0) {
            int left, right;

            left  = *row++;
            right = *row++;
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
    }
    return 0;
}