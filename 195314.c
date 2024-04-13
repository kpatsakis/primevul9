Mat_CalcSubscripts(int rank,int *dims,int index)
{
    int i, j, *subs;
    double l;

    subs = (int*)malloc(rank*sizeof(int));
    l = index;
    for ( i = rank; i--; ) {
        int k = 1;
        for ( j = i; j--; )
            k *= dims[j];
        subs[i] = (int)floor(l / (double)k);
        l -= subs[i]*k;
        subs[i]++;
    }

    return subs;
}