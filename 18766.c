ins_eol(int c)
{
    int	    i;

    if (echeck_abbr(c + ABBR_OFF))
	return OK;
    if (stop_arrow() == FAIL)
	return FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    if ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_push(NUL);

    /*
     * In VREPLACE mode, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    if (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    if (revins_on)
	curwin->w_cursor.col += (colnr_T)STRLEN(ml_get_cursor());
#endif

    AppendToRedobuff(NL_STR);
    i = open_line(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    NULL);
    old_indent = 0;
#ifdef FEAT_CINDENT
    can_cindent = TRUE;
#endif
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    return i;
}