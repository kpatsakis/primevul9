test_keys( ELG_secret_key *sk, unsigned int nbits )
{
    ELG_public_key pk;
    MPI test = mpi_alloc( 0 );
    MPI out1_a = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );
    MPI out1_b = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );
    MPI out2   = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );

    pk.p = sk->p;
    pk.g = sk->g;
    pk.y = sk->y;

    /*mpi_set_bytes( test, nbits, get_random_byte, 0 );*/
    {	char *p = get_random_bits( nbits, 0, 0 );
	mpi_set_buffer( test, p, (nbits+7)/8, 0 );
	xfree(p);
    }

    do_encrypt( out1_a, out1_b, test, &pk );
    decrypt( out2, out1_a, out1_b, sk );
    if( mpi_cmp( test, out2 ) )
	log_fatal("Elgamal operation: encrypt, decrypt failed\n");

    mpi_free( test );
    mpi_free( out1_a );
    mpi_free( out1_b );
    mpi_free( out2 );
}