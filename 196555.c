generate(  ELG_secret_key *sk, unsigned int nbits, MPI **ret_factors )
{
    MPI p;    /* the prime */
    MPI p_min1;
    MPI g;
    MPI x;    /* the secret exponent */
    MPI y;
    MPI temp;
    unsigned int qbits;
    unsigned int xbits;
    byte *rndbuf;

    p_min1 = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );
    temp   = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );
    qbits  = wiener_map ( nbits );
    if( qbits & 1 ) /* better have a even one */
	qbits++;
    g = mpi_alloc(1);
    p = generate_elg_prime( 0, nbits, qbits, g, ret_factors );
    mpi_sub_ui(p_min1, p, 1);


    /* select a random number which has these properties:
     *	 0 < x < p-1
     * This must be a very good random number because this is the
     * secret part.  The prime is public and may be shared anyway,
     * so a random generator level of 1 is used for the prime.
     *
     * I don't see a reason to have a x of about the same size as the
     * p.  It should be sufficient to have one about the size of q or
     * the later used k plus a large safety margin. Decryption will be
     * much faster with such an x.  Note that this is not optimal for
     * signing keys becuase it makes an attack using accidential small
     * K values even easier.  Well, one should not use ElGamal signing
     * anyway.
     */
    xbits = qbits * 3 / 2;
    if( xbits >= nbits )
	BUG();
    x = mpi_alloc_secure ( mpi_nlimb_hint_from_nbits (xbits) );
    if( DBG_CIPHER )
	log_debug("choosing a random x of size %u", xbits );
    rndbuf = NULL;
    do {
	if( DBG_CIPHER )
	    progress('.');
	if( rndbuf ) { /* change only some of the higher bits */
	    if( xbits < 16 ) {/* should never happen ... */
		xfree(rndbuf);
		rndbuf = get_random_bits( xbits, 2, 1 );
	    }
	    else {
		char *r = get_random_bits( 16, 2, 1 );
		memcpy(rndbuf, r, 16/8 );
		xfree(r);
	    }
	}
	else
	    rndbuf = get_random_bits( xbits, 2, 1 );
	mpi_set_buffer( x, rndbuf, (xbits+7)/8, 0 );
	mpi_clear_highbit( x, xbits+1 );
    } while( !( mpi_cmp_ui( x, 0 )>0 && mpi_cmp( x, p_min1 )<0 ) );
    xfree(rndbuf);

    y = mpi_alloc ( mpi_nlimb_hint_from_nbits (nbits) );
    mpi_powm( y, g, x, p );

    if( DBG_CIPHER ) {
	progress('\n');
	log_mpidump("elg  p= ", p );
	log_mpidump("elg  g= ", g );
	log_mpidump("elg  y= ", y );
	log_mpidump("elg  x= ", x );
    }

    /* copy the stuff to the key structures */
    sk->p = p;
    sk->g = g;
    sk->y = y;
    sk->x = x;

    /* now we can test our keys (this should never fail!) */
    test_keys( sk, nbits - 64 );

    mpi_free( p_min1 );
    mpi_free( temp   );
}