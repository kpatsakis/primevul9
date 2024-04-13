init_contour_cursor(const fill_options * const fo, contour_cursor *q)
{
    if (q->pseg->type == s_curve) {
        curve_segment *s = (curve_segment *)q->pseg;
        fixed ymin = min(min(q->prev->pt.y, s->p1.y), min(s->p2.y, s->pt.y));
        fixed ymax = max(max(q->prev->pt.y, s->p1.y), max(s->p2.y, s->pt.y));
        bool in_band = ymin <= fo->ymax && ymax >= fo->ymin;
        q->crossing = ymin < fo->ymin && ymax >= fo->ymin;
        q->monotonic_y = !in_band ||
            (!q->crossing &&
            ((q->prev->pt.y <= s->p1.y && s->p1.y <= s->p2.y && s->p2.y <= s->pt.y) ||
             (q->prev->pt.y >= s->p1.y && s->p1.y >= s->p2.y && s->p2.y >= s->pt.y)));
        q->monotonic_x =
            ((q->prev->pt.x <= s->p1.x && s->p1.x <= s->p2.x && s->p2.x <= s->pt.x) ||
             (q->prev->pt.x >= s->p1.x && s->p1.x >= s->p2.x && s->p2.x >= s->pt.x));
    } else
        q->monotonic_y = true;
    if (!q->monotonic_y) {
        curve_segment *s = (curve_segment *)q->pseg;
        int k = gx_curve_log2_samples(q->prev->pt.x, q->prev->pt.y, s, fo->fixed_flat);

        if (!gx_flattened_iterator__init(&q->fi, q->prev->pt.x, q->prev->pt.y, s, k))
            return_error(gs_error_rangecheck);
    } else {
        q->dir = compute_dir(fo, q->prev->pt.y, q->pseg->pt.y);
        gx_flattened_iterator__init_line(&q->fi,
            q->prev->pt.x, q->prev->pt.y, q->pseg->pt.x, q->pseg->pt.y); /* fake for curves. */
    }
    q->first_flattened = true;
    return 0;
}