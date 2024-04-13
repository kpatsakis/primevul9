static void js_dumpproperty(js_State *J, js_Property *node)
{
	minify = 0;
	if (node->left->level)
		js_dumpproperty(J, node->left);
	printf("\t%s: ", node->name);
	js_dumpvalue(J, node->value);
	printf(",\n");
	if (node->right->level)
		js_dumpproperty(J, node->right);
}