parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    int		n;
    int		divider;
    int		fraction = 0;
    int		sw = (int)get_sw_value(buf);

    // Set the default values.

    // Spaces from a block's opening brace the prevailing indent for that
    // block should be.
    buf->b_ind_level = sw;

    // Spaces from the edge of the line an open brace that's at the end of a
    // line is imagined to be.
    buf->b_ind_open_imag = 0;

    // Spaces from the prevailing indent for a line that is not preceded by
    // an opening brace.
    buf->b_ind_no_brace = 0;

    // Column where the first { of a function should be located }.
    buf->b_ind_first_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_left_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_code = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_code = sw;

    // Amount K&R-style parameters should be indented.
    buf->b_ind_param = sw;

    // Amount a function type spec should be indented.
    buf->b_ind_func_type = sw;

    // Amount a cpp base class declaration or constructor initialization
    // should be indented.
    buf->b_ind_cpp_baseclass = sw;

    // additional spaces beyond the prevailing indent a continuation line
    // should be located.
    buf->b_ind_continuation = sw;

    // Spaces from the indent of the line with an unclosed parenthesis.
    buf->b_ind_unclosed = sw * 2;

    // Spaces from the indent of the line with an unclosed parenthesis, which
    // itself is also unclosed.
    buf->b_ind_unclosed2 = sw;

    // Suppress ignoring spaces from the indent of a line starting with an
    // unclosed parenthesis.
    buf->b_ind_unclosed_noignore = 0;

    // If the opening paren is the last nonwhite character on the line, and
    // b_ind_unclosed_wrapped is nonzero, use this indent relative to the outer
    // context (for very long lines).
    buf->b_ind_unclosed_wrapped = 0;

    // Suppress ignoring white space when lining up with the character after
    // an unclosed parenthesis.
    buf->b_ind_unclosed_whiteok = 0;

    // Indent a closing parenthesis under the line start of the matching
    // opening parenthesis.
    buf->b_ind_matching_paren = 0;

    // Indent a closing parenthesis under the previous line.
    buf->b_ind_paren_prev = 0;

    // Extra indent for comments.
    buf->b_ind_comment = 0;

    // Spaces from the comment opener when there is nothing after it.
    buf->b_ind_in_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_in_comment2 = 0;

    // Max lines to search for an open paren.
    buf->b_ind_maxparen = 20;

    // Max lines to search for an open comment.
    buf->b_ind_maxcomment = 70;

    // Handle braces for java code.
    buf->b_ind_java = 0;

    // Not to confuse JS object properties with labels.
    buf->b_ind_js = 0;

    // Handle blocked cases correctly.
    buf->b_ind_keep_case_label = 0;

    // Handle C++ namespace.
    buf->b_ind_cpp_namespace = 0;

    // Handle continuation lines containing conditions of if(), for() and
    // while().
    buf->b_ind_if_for_while = 0;

    // indentation for # comments
    buf->b_ind_hash_comment = 0;

    // Handle C++ extern "C" or "C++"
    buf->b_ind_cpp_extern_c = 0;

    // Handle C #pragma directives
    buf->b_ind_pragma = 0;

    for (p = buf->b_p_cino; *p; )
    {
	l = p++;
	if (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	if (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((char *)++p);
	    while (VIM_ISDIGIT(*p))
	    {
		++p;
		if (divider)
		    divider *= 10;
		else
		    divider = 10;
	    }
	}
	if (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    if (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    else
	    {
		n *= sw;
		if (divider)
		    n += (sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	if (l[1] == '-')
	    n = -n;

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	switch (*l)
	{
	    case '>': buf->b_ind_level = n; break;
	    case 'e': buf->b_ind_open_imag = n; break;
	    case 'n': buf->b_ind_no_brace = n; break;
	    case 'f': buf->b_ind_first_open = n; break;
	    case '{': buf->b_ind_open_extra = n; break;
	    case '}': buf->b_ind_close_extra = n; break;
	    case '^': buf->b_ind_open_left_imag = n; break;
	    case 'L': buf->b_ind_jump_label = n; break;
	    case ':': buf->b_ind_case = n; break;
	    case '=': buf->b_ind_case_code = n; break;
	    case 'b': buf->b_ind_case_break = n; break;
	    case 'p': buf->b_ind_param = n; break;
	    case 't': buf->b_ind_func_type = n; break;
	    case '/': buf->b_ind_comment = n; break;
	    case 'c': buf->b_ind_in_comment = n; break;
	    case 'C': buf->b_ind_in_comment2 = n; break;
	    case 'i': buf->b_ind_cpp_baseclass = n; break;
	    case '+': buf->b_ind_continuation = n; break;
	    case '(': buf->b_ind_unclosed = n; break;
	    case 'u': buf->b_ind_unclosed2 = n; break;
	    case 'U': buf->b_ind_unclosed_noignore = n; break;
	    case 'W': buf->b_ind_unclosed_wrapped = n; break;
	    case 'w': buf->b_ind_unclosed_whiteok = n; break;
	    case 'm': buf->b_ind_matching_paren = n; break;
	    case 'M': buf->b_ind_paren_prev = n; break;
	    case ')': buf->b_ind_maxparen = n; break;
	    case '*': buf->b_ind_maxcomment = n; break;
	    case 'g': buf->b_ind_scopedecl = n; break;
	    case 'h': buf->b_ind_scopedecl_code = n; break;
	    case 'j': buf->b_ind_java = n; break;
	    case 'J': buf->b_ind_js = n; break;
	    case 'l': buf->b_ind_keep_case_label = n; break;
	    case '#': buf->b_ind_hash_comment = n; break;
	    case 'N': buf->b_ind_cpp_namespace = n; break;
	    case 'k': buf->b_ind_if_for_while = n; break;
	    case 'E': buf->b_ind_cpp_extern_c = n; break;
	    case 'P': buf->b_ind_pragma = n; break;
	}
	if (*p == ',')
	    ++p;
    }
}