elg_decrypt( int algo, MPI *result, MPI *data, MPI *skey )
{
    ELG_secret_key sk;

    if( !is_ELGAMAL(algo) )
	return G10ERR_PUBKEY_ALGO;
    if( !data[0] || !data[1]
	|| !skey[0] || !skey[1] || !skey[2] || !skey[3] )
	return G10ERR_BAD_MPI;

    sk.p = skey[0];
    sk.g = skey[1];
    sk.y = skey[2];
    sk.x = skey[3];
    *result = mpi_alloc_secure( mpi_get_nlimbs( sk.p ) );
    decrypt( *result, data[0], data[1], &sk );
    return 0;
}