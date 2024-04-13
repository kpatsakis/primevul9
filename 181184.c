copy_clientid(clientid_t *clid, struct nfsd4_session *session)
{
	struct nfsd4_sessionid *sid =
			(struct nfsd4_sessionid *)session->se_sessionid.data;

	clid->cl_boot = sid->clientid.cl_boot;
	clid->cl_id = sid->clientid.cl_id;
}