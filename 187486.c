op_formatexpr(oparg_T *oap)
{
    if (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(INVERTED);

    if (fex_format(oap->start.lnum, oap->line_count, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}