getnextcomp(char_u *fname)
{
    while (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    if (*fname)
	++fname;
    return fname;
}