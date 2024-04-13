static bool qpd_cmp_func(const void *obj, const void *userp)
{
    const QpdEntry *e1 = obj, *e2 = userp;
    return e1->dev == e2->dev;
}