static bool qpp_cmp_func(const void *obj, const void *userp)
{
    const QppEntry *e1 = obj, *e2 = userp;
    return e1->dev == e2->dev && e1->ino_prefix == e2->ino_prefix;
}