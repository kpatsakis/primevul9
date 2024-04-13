modifiers2keycode(int modifiers, int *key, char_u *string)
{
    int new_slen = 0;

    if (modifiers != 0)
    {
	// Some keys have the modifier included.  Need to handle that here to
	// make mappings work.  This may result in a special key, such as
	// K_S_TAB.
	*key = simplify_key(*key, &modifiers);
	if (modifiers != 0)
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = (int)KS_MODIFIER;
	    string[new_slen++] = modifiers;
	}
    }
    return new_slen;
}