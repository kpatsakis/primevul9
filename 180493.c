static int edgecmp(const void *a, const void *b)
{
    int left  = ((int*)a)[0]&~1;
    int right = ((int*)b)[0]&~1;
    if (left != right)
        return left - right;
    return ((int*)a)[1] - ((int*)b)[1];
}