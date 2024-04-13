nfs41_same_server_scope(struct nfs41_server_scope *a,
			struct nfs41_server_scope *b)
{
	if (a->server_scope_sz == b->server_scope_sz &&
	    memcmp(a->server_scope, b->server_scope, a->server_scope_sz) == 0)
		return true;

	return false;
}