int gx_scan_convert_tr(gx_device     * pdev,
                       gx_path       * path,
                 const gs_fixed_rect * clip,
                       gx_edgebuffer * edgebuffer,
                       fixed           fixed_flat)
{
    gs_fixed_rect  ibox;
    int            scanlines;
    const subpath *psub;
    int           *index;
    int           *table;
    int            i;
    int            code;
    int            id = 0;

    edgebuffer->index = NULL;
    edgebuffer->table = NULL;

    /* Bale out if no actual path. We see this with the clist */
    if (path->first_subpath == NULL)
        return 0;

    code = make_bbox(path, clip, &ibox, fixed_half);
    if (code < 0)
        return code;

    if (ibox.q.y <= ibox.p.y)
        return 0;

    code = make_table(pdev, path, &ibox, 2, fixed_half, &scanlines, &index, &table);
    if (code < 0)
        return code;

    /* Step 3: Now we run through the path, filling in the real
     * values. */
    for (psub = path->first_subpath; psub != 0;) {
        const segment *pseg = (const segment *)psub;
        fixed ex = pseg->pt.x;
        fixed ey = pseg->pt.y;
        fixed ix = ex;
        fixed iy = ey;

        while ((pseg = pseg->next) != 0 &&
               pseg->type != s_start
            ) {
            fixed sx = ex;
            fixed sy = ey;
            ex = pseg->pt.x;
            ey = pseg->pt.y;

            switch (pseg->type) {
                default:
                case s_start: /* Should never happen */
                case s_dash:  /* We should never be seeing a dash here */
                    assert("This should never happen" == NULL);
                    break;
                case s_curve: {
                    const curve_segment *const pcur = (const curve_segment *)pseg;
                    int k = gx_curve_log2_samples(sx, sy, pcur, fixed_flat);

                    mark_curve_tr(sx, sy, pcur->p1.x, pcur->p1.y, pcur->p2.x, pcur->p2.y, ex, ey, ibox.p.y, scanlines, table, index, &id, k);
                    break;
                }
                case s_gap:
                case s_line:
                case s_line_close:
                    if (sy != ey)
                        mark_line_tr(sx, sy, ex, ey, ibox.p.y, scanlines, table, index, ++id);
                    break;
            }
        }
        /* And close any open segments */
        if (iy != ey)
            mark_line_tr(ex, ey, ix, iy, ibox.p.y, scanlines, table, index, ++id);
        psub = (const subpath *)pseg;
    }

    /* Step 2 complete: We now have a complete list of intersection data in
     * table, indexed by index. */

    /* Step 4: Sort the intersects on x */
    for (i=0; i < scanlines; i++) {
        int *row = &table[index[i]];
        int  rowlen = *row++;

        /* Sort the 'rowlen' entries - *must* be faster to do this with
         * custom code, as the function call overhead on the comparisons
         * will be a killer */
        qsort(row, rowlen, 2*sizeof(int), intcmp_tr);
    }

    edgebuffer->base   = ibox.p.y;
    edgebuffer->height = scanlines;
    edgebuffer->xmin   = ibox.p.x;
    edgebuffer->xmax   = ibox.q.x;
    edgebuffer->index  = index;
    edgebuffer->table  = table;

    return 0;
}