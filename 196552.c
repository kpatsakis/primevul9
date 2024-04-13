elg_encrypt( int algo, MPI *resarr, MPI data, MPI *pkey )
{
    ELG_public_key pk;

    if( !is_ELGAMAL(algo) )
	return G10ERR_PUBKEY_ALGO;
    if( !data || !pkey[0] || !pkey[1] || !pkey[2] )
	return G10ERR_BAD_MPI;

    pk.p = pkey[0];
    pk.g = pkey[1];
    pk.y = pkey[2];
    resarr[0] = mpi_alloc( mpi_get_nlimbs( pk.p ) );
    resarr[1] = mpi_alloc( mpi_get_nlimbs( pk.p ) );
    do_encrypt( resarr[0], resarr[1], data, &pk );
    return 0;
}