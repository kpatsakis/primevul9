Perl__invlist_dump(pTHX_ PerlIO *file, I32 level,
                         const char * const indent, SV* const invlist)
{
    /* Designed to be called only by do_sv_dump().  Dumps out the ranges of the
     * inversion list 'invlist' to 'file' at 'level'  Each line is prefixed by
     * the string 'indent'.  The output looks like this:
         [0] 0x000A .. 0x000D
         [2] 0x0085
         [4] 0x2028 .. 0x2029
         [6] 0x3104 .. INFINITY
     * This means that the first range of code points matched by the list are
     * 0xA through 0xD; the second range contains only the single code point
     * 0x85, etc.  An inversion list is an array of UVs.  Two array elements
     * are used to define each range (except if the final range extends to
     * infinity, only a single element is needed).  The array index of the
     * first element for the corresponding range is given in brackets. */

    UV start, end;
    STRLEN count = 0;

    PERL_ARGS_ASSERT__INVLIST_DUMP;

    if (invlist_is_iterating(invlist)) {
        Perl_dump_indent(aTHX_ level, file,
             "%sCan't dump inversion list because is in middle of iterating\n",
             indent);
        return;
    }

    invlist_iterinit(invlist);
    while (invlist_iternext(invlist, &start, &end)) {
	if (end == UV_MAX) {
	    Perl_dump_indent(aTHX_ level, file,
                                       "%s[%" UVuf "] 0x%04" UVXf " .. INFINITY\n",
                                   indent, (UV)count, start);
	}
	else if (end != start) {
	    Perl_dump_indent(aTHX_ level, file,
                                    "%s[%" UVuf "] 0x%04" UVXf " .. 0x%04" UVXf "\n",
		                indent, (UV)count, start,         end);
	}
	else {
	    Perl_dump_indent(aTHX_ level, file, "%s[%" UVuf "] 0x%04" UVXf "\n",
                                            indent, (UV)count, start);
	}
        count += 2;
    }
}