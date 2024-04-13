valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    if (enc_utf8 && !utf_valid_string(word, end))
	return FALSE;
    for (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	if (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    return FALSE;
    return TRUE;
}