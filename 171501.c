void gnutls_x509_policy_release(struct gnutls_x509_policy_st *policy)
{
	unsigned i;

	gnutls_free(policy->oid);
	for (i = 0; i < policy->qualifiers; i++)
		gnutls_free(policy->qualifier[i].data);
}