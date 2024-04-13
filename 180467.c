static int edgecmp_tr(const void *a, const void *b)
{
    int left  = ((int*)a)[0];
    int right = ((int*)b)[0];
    if (left != right)
        return left - right;
    left = ((int*)a)[1] - ((int*)b)[1];
    if (left != 0)
        return left;
    left = ((int*)a)[2] - ((int*)b)[2];
    if (left != 0)
        return left;
    return ((int*)a)[3] - ((int*)b)[3];
}