Mat_CalcSubscripts2(int rank,size_t *dims,size_t index)
{
    int i;
    size_t *subs;
    double l;

    subs = (size_t*)malloc(rank*sizeof(size_t));
    l = (double)index;
    for ( i = rank; i--; ) {
        int j;
        size_t k = 1;
        for ( j = i; j--; )
            k *= dims[j];
        subs[i] = (size_t)floor(l / (double)k);
        l -= subs[i]*k;
        subs[i]++;
    }

    return subs;
}