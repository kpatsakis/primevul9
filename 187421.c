static void mdesc_free(struct mdesc_handle *hp)
{
	hp->mops->free(hp);
}