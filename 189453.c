int jpc_enc_encpkts(jpc_enc_t *enc, jas_stream_t *out)
{
	jpc_enc_tile_t *tile;
	jpc_pi_t *pi;

	tile = enc->curtile;

	jpc_init_t2state(enc, 0);
	pi = tile->pi;
	jpc_pi_init(pi);

	if (!jpc_pi_next(pi)) {
		for (;;) {
			if (jpc_enc_encpkt(enc, out, jpc_pi_cmptno(pi), jpc_pi_rlvlno(pi),
			  jpc_pi_prcno(pi), jpc_pi_lyrno(pi))) {
				return -1;
			}
			if (jpc_pi_next(pi)) {
				break;
			}
		}
	}
	
	return 0;
}