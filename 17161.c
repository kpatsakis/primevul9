add_pathsep(char_u *p)
{
    if (*p != NUL && !after_pathsep(p, p + STRLEN(p)))
	STRCAT(p, PATHSEPSTR);
}