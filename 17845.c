tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    int		copyID)
{
    return echo_string_core(tv, tofree, numbuf, copyID, FALSE, TRUE, FALSE);
}