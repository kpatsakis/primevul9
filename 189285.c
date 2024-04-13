check_opt_strings(
    char_u	*val,
    char	**values,
    int		list)	    /* when TRUE: accept a list of values */
{
    return opt_strings_flags(val, values, NULL, list);
}