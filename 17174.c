mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    int		flags)		// EW_* flags
{
    return dos_expandpath(gap, path, 0, flags, FALSE);
}