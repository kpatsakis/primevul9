Mat_CalcSingleSubscript2(int rank,size_t *dims,size_t *subs,size_t *index)
{
    int i, err = 0;

    for ( i = 0; i < rank; i++ ) {
        int j;
        size_t k = subs[i];
        if ( k > dims[i] ) {
            err = 1;
            Mat_Critical("Mat_CalcSingleSubscript2: index out of bounds");
            break;
        } else if ( k < 1 ) {
            err = 1;
            break;
        }
        k--;
        for ( j = i; j--; )
            k *= dims[j];
        *index += k;
    }

    return err;
}