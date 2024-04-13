static void swap_binders(gnutls_session_t session)
{
	struct binder_data_st tmp;

	memcpy(&tmp, &session->key.binders[0], sizeof(struct binder_data_st));
	memcpy(&session->key.binders[0], &session->key.binders[1], sizeof(struct binder_data_st));
	memcpy(&session->key.binders[1], &tmp, sizeof(struct binder_data_st));
}