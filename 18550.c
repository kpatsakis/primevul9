hide_stolen_goods_from_cms(cms_context *new,
			   cms_context *old UNUSED)
{
	new->tokenname = NULL;
	new->certname = NULL;
}