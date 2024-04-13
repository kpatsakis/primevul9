cmove(T_ASC_Association *assoc, const char *fname)
{
    OFCondition cond = EC_Normal;
    int n = OFstatic_cast(int, opt_repeatCount);
    while (cond.good() && n--)
        cond = moveSCU(assoc, fname);
    return cond;
}