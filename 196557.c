elg_get_nbits( int algo, MPI *pkey )
{
    if( !is_ELGAMAL(algo) )
	return 0;
    return mpi_get_nbits( pkey[0] );
}