static void deinit_keys(gnutls_session_t session)
{
	gnutls_pk_params_release(&session->key.ecdh_params);
	gnutls_pk_params_release(&session->key.dh_params);
	zrelease_temp_mpi_key(&session->key.ecdh_x);
	zrelease_temp_mpi_key(&session->key.ecdh_y);
	_gnutls_free_temp_key_datum(&session->key.ecdhx);

	zrelease_temp_mpi_key(&session->key.client_Y);

	/* SRP */
	zrelease_temp_mpi_key(&session->key.srp_p);
	zrelease_temp_mpi_key(&session->key.srp_g);
	zrelease_temp_mpi_key(&session->key.srp_key);

	zrelease_temp_mpi_key(&session->key.u);
	zrelease_temp_mpi_key(&session->key.a);
	zrelease_temp_mpi_key(&session->key.x);
	zrelease_temp_mpi_key(&session->key.A);
	zrelease_temp_mpi_key(&session->key.B);
	zrelease_temp_mpi_key(&session->key.b);

	_gnutls_free_temp_key_datum(&session->key.key);
	_gnutls_free_temp_key_datum(&session->key.key);
}