cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    int		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*line = ml_get_curline();

    if (pos->lnum <= lnum)
	return cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(line);
    if (*s == '#')		// skip #define FOO x ? (x) : x
	return FALSE;
    s = cin_skipcomment(s);
    if (*s == NUL)
	return FALSE;

    cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;

    // Search for a line starting with '#', empty, ending in ';' or containing
    // '{' or '}' and start below it.  This handles the following situations:
    //	a = cond ?
    //	      func() :
    //		   asdf;
    //	func::foo()
    //	      : something
    //	{}
    //	Foo::Foo (int one, int two)
    //		: something(4),
    //		somethingelse(3)
    //	{}
    while (lnum > 1)
    {
	line = ml_get(lnum - 1);
	s = skipwhite(line);
	if (*s == '#' || *s == NUL)
	    break;
	while (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    if (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		break;
	    if (*s != NUL)
		++s;
	}
	if (*s != NUL)
	    break;
	--lnum;
    }

    pos->lnum = lnum;
    line = ml_get(lnum);
    s = line;
    for (;;)
    {
	if (*s == NUL)
	{
	    if (lnum == curwin->w_cursor.lnum)
		break;
	    // Continue in the cursor line.
	    line = ml_get(++lnum);
	    s = line;
	}
	if (s == line)
	{
	    // don't recognize "case (foo):" as a baseclass
	    if (cin_iscase(s, FALSE))
		break;
	    s = cin_skipcomment(line);
	    if (*s == NUL)
		continue;
	}

	if (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	else if (s[0] == ':')
	{
	    if (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    else if (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    else
		s = cin_skipcomment(s + 1);
	}
	else if ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    if (*s == 'c')
		s = cin_skipcomment(s + 5);
	    else
		s = cin_skipcomment(s + 6);
	}
	else
	{
	    if (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    else if (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    else if (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		return FALSE;
	    }
	    else if (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    else if (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		if (cpp_base_class)
		    pos->col = (colnr_T)(s - line);
	    }

	    // When the line ends in a comma don't align with it.
	    if (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    if (cpp_base_class)
	pos->lnum = lnum;
    return cpp_base_class;
}