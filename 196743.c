to_name_end(char_u *arg, int use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    if (!eval_isnamec1(*arg))
	return arg;

    for (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	if (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == NULL))
	    break;
    return p;
}