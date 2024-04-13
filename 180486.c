add_y_curve_part(line_list *ll, segment *s0, segment *s1, int dir,
    gx_flattened_iterator *fi, bool more1, bool step_back, bool monotonic_x)
{
    active_line *alp = make_al(ll);
    int code;

    if (alp == NULL)
        return_error(gs_error_VMerror);
    alp->pseg = (dir == DIR_UP ? s1 : s0);
    alp->direction = dir;
    alp->fi = *fi;
    alp->more_flattened = more1;
    if (dir != DIR_UP && more1)
        gx_flattened_iterator__switch_to_backscan(&alp->fi, more1);
    if (step_back) {
        do {
            code = gx_flattened_iterator__prev(&alp->fi);
            if (code < 0)
                return code;
            alp->more_flattened = code;
            if (compute_dir(ll->fo, alp->fi.ly0, alp->fi.ly1) != 2)
                break;
        } while (alp->more_flattened);
    }
    code = step_al(alp, false);
    if (code < 0)
        return code;
    alp->monotonic_y = false;
    alp->monotonic_x = monotonic_x;
    insert_y_line(ll, alp);
    return 0;
}