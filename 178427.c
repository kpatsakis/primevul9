do_Upper(int *d, int c)
{
    *d = MB_TOUPPER(c);

    return (fptr_T)do_Upper;
}