elg_generate( int algo, unsigned nbits, MPI *skey, MPI **retfactors )
{
    ELG_secret_key sk;

    if( !is_ELGAMAL(algo) )
	return G10ERR_PUBKEY_ALGO;

    generate( &sk, nbits, retfactors );
    skey[0] = sk.p;
    skey[1] = sk.g;
    skey[2] = sk.y;
    skey[3] = sk.x;
    return 0;
}