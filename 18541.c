steal_from_cms(cms_context *old, cms_context *new)
{
	new->tokenname = old->tokenname;
	new->certname = old->certname;

	new->selected_digest = old->selected_digest;

	new->log = old->log;
	new->log_priv = old->log_priv;
}