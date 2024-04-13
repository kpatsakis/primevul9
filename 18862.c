handle_key_with_modifier(
	int	*arg,
	int	trail,
	int	csi_len,
	int	offset,
	char_u	*buf,
	int	bufsize,
	int	*buflen)
{
    int	    key;
    int	    modifiers;
    int	    new_slen;
    char_u  string[MAX_KEY_CODE_LEN + 1];

    seenModifyOtherKeys = TRUE;
    if (trail == 'u')
	key = arg[0];
    else
	key = arg[2];

    modifiers = decode_modifiers(arg[1]);

    // Some keys need adjustment when the Ctrl modifier is used.
    key = may_adjust_key_for_ctrl(modifiers, key);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_shift_modifier(modifiers, key);

    // insert modifiers with KS_MODIFIER
    new_slen = modifiers2keycode(modifiers, &key, string);

    if (IS_SPECIAL(key))
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = KEY2TERMCAP0(key);
	string[new_slen++] = KEY2TERMCAP1(key);
    }
    else if (has_mbyte)
	new_slen += (*mb_char2bytes)(key, string + new_slen);
    else
	string[new_slen++] = key;

    if (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	return -1;
    return new_slen - csi_len + offset;
}