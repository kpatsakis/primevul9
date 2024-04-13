static int compare_uint(const void *_a, const void *_b)
{
    unsigned int a = *(unsigned int *)_a;
    unsigned int b = *(unsigned int *)_b;

    return a < b ? -1 : a > b ? 1 : 0;
}