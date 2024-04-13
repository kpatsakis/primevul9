do_Lower(int *d, int c)
{
    *d = MB_TOLOWER(c);

    return (fptr_T)do_Lower;
}