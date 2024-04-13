concat_fnames(char_u *fname1, char_u *fname2, int sep)
{
    char_u  *dest;

    dest = alloc(STRLEN(fname1) + STRLEN(fname2) + 3);
    if (dest != NULL)
    {
	STRCPY(dest, fname1);
	if (sep)
	    add_pathsep(dest);
	STRCAT(dest, fname2);
    }
    return dest;
}