x_order(const active_line *lp1, const active_line *lp2)
{
    bool s1;

    INCR(order);
    if (!lp1 || !lp2 || lp1->x_current < lp2->x_current)
        return -1;
    else if (lp1->x_current > lp2->x_current)
        return 1;
    /*
     * We need to compare the slopes of the lines.  Start by
     * checking one fast case, where the slopes have opposite signs.
     */
    if ((s1 = lp1->start.x < lp1->end.x) != (lp2->start.x < lp2->end.x))
        return (s1 ? 1 : -1);
    /*
     * We really do have to compare the slopes.  Fortunately, this isn't
     * needed very often.  We want the sign of the comparison
     * dx1/dy1 - dx2/dy2, or (since we know dy1 and dy2 are positive)
     * dx1 * dy2 - dx2 * dy1.  In principle, we can't simply do this using
     * doubles, since we need complete accuracy and doubles don't have
     * enough fraction bits.  However, with the usual 20+12-bit fixeds and
     * 64-bit doubles, both of the factors would have to exceed 2^15
     * device space pixels for the result to be inaccurate, so we
     * simply disregard this possibility.  ****** FIX SOMEDAY ******
     */
    INCR(slow_order);
    {
        fixed dx1 = lp1->end.x - lp1->start.x,
            dy1 = lp1->end.y - lp1->start.y;
        fixed dx2 = lp2->end.x - lp2->start.x,
            dy2 = lp2->end.y - lp2->start.y;
        double diff = (double)dx1 * dy2 - (double)dx2 * dy1;

        return (diff < 0 ? -1 : diff > 0 ? 1 : 0);
    }
}