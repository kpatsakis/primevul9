skipchr_keepstart(void)
{
    int as = prev_at_start;
    int pr = prevchr;
    int prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}