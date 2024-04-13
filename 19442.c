mouse_has(int c)
{
    char_u	*p;

    for (p = p_mouse; *p; ++p)
	switch (*p)
	{
	    case 'a': if (vim_strchr((char_u *)MOUSE_A, c) != NULL)
			  return TRUE;
		      break;
	    case MOUSE_HELP: if (c != MOUSE_RETURN && curbuf->b_help)
				 return TRUE;
			     break;
	    default: if (c == *p) return TRUE; break;
	}
    return FALSE;
}