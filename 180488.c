add_y_line(const segment * prev_lp, const segment * lp, int dir, line_list *ll)
{
    return add_y_line_aux(prev_lp, lp, &lp->pt, &prev_lp->pt, dir, ll);
}