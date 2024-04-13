start_al_pair_from_min(line_list *ll, contour_cursor *q)
{
    int dir, code;
    const fill_options * const fo = ll->fo;

    /* q stands at the first segment, which isn't last. */
    do {
        code = gx_flattened_iterator__next(&q->fi);
        if (code < 0)
            return code;
        q->more_flattened = code;
        dir = compute_dir(fo, q->fi.ly0, q->fi.ly1);
        if (q->fi.ly0 > fo->ymax && ll->y_break > q->fi.y0)
            ll->y_break = q->fi.ly0;
        if (q->fi.ly1 > fo->ymax && ll->y_break > q->fi.ly1)
            ll->y_break = q->fi.ly1;
        if (q->fi.ly0 >= fo->ymin) {
            if (dir == DIR_UP && ll->main_dir == DIR_DOWN) {
                code = add_y_curve_part(ll, q->prev, q->pseg, DIR_DOWN, &q->fi,
                                        true, true, q->monotonic_x);
                if (code < 0)
                    return code;
                code = add_y_curve_part(ll, q->prev, q->pseg, DIR_UP, &q->fi,
                                        q->more_flattened, false, q->monotonic_x);
                if (code < 0)
                    return code;
            } else if (q->fi.ly1 < fo->ymin) {
                code = add_y_curve_part(ll, q->prev, q->pseg, DIR_DOWN, &q->fi,
                                        true, false, q->monotonic_x);
                if (code < 0)
                    return code;
            }
        } else if (q->fi.ly1 >= fo->ymin) {
            code = add_y_curve_part(ll, q->prev, q->pseg, DIR_UP, &q->fi,
                                    q->more_flattened, false, q->monotonic_x);
            if (code < 0)
                return code;
        }
        q->first_flattened = false;
        q->dir = dir;
        if (dir == DIR_DOWN || dir == DIR_UP)
            ll->main_dir = dir;
    } while(q->more_flattened);
    /* q stands at the last segment. */
    return 0;
    /* note : it doesn't depend on the number of curve minimums,
       which may vary due to arithmetic errors. */
}