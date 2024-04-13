start_al_pair(line_list *ll, contour_cursor *q, contour_cursor *p)
{
    int code;

    if (q->monotonic_y)
        code = add_y_line(q->prev, q->pseg, DIR_DOWN, ll);
    else
        code = add_y_curve_part(ll, q->prev, q->pseg, DIR_DOWN, &q->fi,
                            !q->first_flattened, false, q->monotonic_x);
    if (code < 0)
        return code;
    if (p->monotonic_y)
        code = add_y_line(p->prev, p->pseg, DIR_UP, ll);
    else
        code = add_y_curve_part(ll, p->prev, p->pseg, DIR_UP, &p->fi,
                            p->more_flattened, false, p->monotonic_x);
    return code;
}