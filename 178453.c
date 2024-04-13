re_multiline(regprog_T *prog)
{
    return (prog->regflags & RF_HASNL);
}