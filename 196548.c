elg_check_secret_key( int algo, MPI *skey )
{
    ELG_secret_key sk;

    if( !is_ELGAMAL(algo) )
	return G10ERR_PUBKEY_ALGO;
    if( !skey[0] || !skey[1] || !skey[2] || !skey[3] )
	return G10ERR_BAD_MPI;

    sk.p = skey[0];
    sk.g = skey[1];
    sk.y = skey[2];
    sk.x = skey[3];
    if( !check_secret_key( &sk ) )
	return G10ERR_BAD_SECKEY;

    return 0;
}