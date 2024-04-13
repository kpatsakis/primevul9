Mat_CalcSingleSubscript(int rank,int *dims,int *subs)
{
    int index = 0, i, j, err = 0;

    for ( i = 0; i < rank; i++ ) {
        int k = subs[i];
        if ( k > dims[i] ) {
            err = 1;
            Mat_Critical("Mat_CalcSingleSubscript: index out of bounds");
            break;
        } else if ( k < 1 ) {
            err = 1;
            break;
        }
        k--;
        for ( j = i; j--; )
            k *= dims[j];
        index += k;
    }
    if ( err )
        index = -1;

    return index;
}