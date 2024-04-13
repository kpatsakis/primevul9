static int end_expression_next(void)
{
    struct tokenval tv;
    char *p;
    int i;

    p = stdscan_get();
    i = stdscan(NULL, &tv);
    stdscan_set(p);

    return (i == ',' || i == ';' || i == ')' || !i);
}