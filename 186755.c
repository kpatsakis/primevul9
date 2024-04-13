check_fontset_extents(
    XCharStruct		*overall,
    int			*logical_ascent,
    int                 *logical_descent,
    XFontStruct		*font)
{
    overall->lbearing = min(overall->lbearing, font->min_bounds.lbearing);
    overall->rbearing = max(overall->rbearing, font->max_bounds.rbearing);
    overall->ascent   = max(overall->ascent,   font->max_bounds.ascent);
    overall->descent  = max(overall->descent,  font->max_bounds.descent);
    overall->width    = max(overall->width,    font->max_bounds.width);
    *logical_ascent   = max(*logical_ascent,   font->ascent);
    *logical_descent  = max(*logical_descent,  font->descent);
}