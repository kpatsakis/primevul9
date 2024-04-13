static bool qpf_cmp_func(const void *obj, const void *userp)
{
    const QpfEntry *e1 = obj, *e2 = userp;
    return e1->dev == e2->dev && e1->ino == e2->ino;
}