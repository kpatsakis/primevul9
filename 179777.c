cin_is_if_for_while_before_offset(char_u *line, int *poffset)
{
    int offset = *poffset;

    if (offset-- < 2)
	return 0;
    while (offset > 2 && VIM_ISWHITE(line[offset]))
	--offset;

    offset -= 1;
    if (!STRNCMP(line + offset, "if", 2))
	goto probablyFound;

    if (offset >= 1)
    {
	offset -= 1;
	if (!STRNCMP(line + offset, "for", 3))
	    goto probablyFound;

	if (offset >= 2)
	{
	    offset -= 2;
	    if (!STRNCMP(line + offset, "while", 5))
		goto probablyFound;
	}
    }
    return 0;

probablyFound:
    if (!offset || !vim_isIDc(line[offset - 1]))
    {
	*poffset = offset;
	return 1;
    }
    return 0;
}