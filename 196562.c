check_secret_key( ELG_secret_key *sk )
{
    int rc;
    MPI y = mpi_alloc( mpi_get_nlimbs(sk->y) );

    mpi_powm( y, sk->g, sk->x, sk->p );
    rc = !mpi_cmp( y, sk->y );
    mpi_free( y );
    return rc;
}