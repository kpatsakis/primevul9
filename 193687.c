help_heuristic(
    char_u	*matched_string,
    int		offset,			// offset for match
    int		wrong_case)		// no matching case
{
    int		num_letters;
    char_u	*p;

    num_letters = 0;
    for (p = matched_string; *p; p++)
	if (ASCII_ISALNUM(*p))
	    num_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    if (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    else if (offset > 2)
	offset *= 200;
    if (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    if (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    return (int)(100 * num_letters + STRLEN(matched_string) + offset);
}