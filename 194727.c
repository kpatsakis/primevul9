static void textview_add_parts(TextView *textview, MimeInfo *mimeinfo)
{
	cm_return_if_fail(mimeinfo != NULL);
	cm_return_if_fail(mimeinfo->node != NULL);

	recursive_add_parts(textview, mimeinfo->node);
}