elg_get_info( int algo, int *npkey, int *nskey, int *nenc, int *nsig,
							 int *use )
{
    *npkey = 3;
    *nskey = 4;
    *nenc = 2;
    *nsig = 2;

    switch( algo ) {
      case PUBKEY_ALGO_ELGAMAL_E:
	*use = PUBKEY_USAGE_ENC;
	return "ELG-E";
      default: *use = 0; return NULL;
    }
}