skipchr(void)
{
    // peekchr() eats a backslash, do the same here
    if (*regparse == '\\')
	prevchr_len = 1;
    else
	prevchr_len = 0;
    if (regparse[prevchr_len] != NUL)
    {
	if (enc_utf8)
	    // exclude composing chars that mb_ptr2len does include
	    prevchr_len += utf_ptr2len(regparse + prevchr_len);
	else if (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	else
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    prev_at_start = at_start;
    at_start = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}