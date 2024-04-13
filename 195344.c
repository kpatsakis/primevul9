int vfs_parse_fs_param_source(struct fs_context *fc, struct fs_parameter *param)
{
	if (strcmp(param->key, "source") != 0)
		return -ENOPARAM;

	if (param->type != fs_value_is_string)
		return invalf(fc, "Non-string source");

	if (fc->source)
		return invalf(fc, "Multiple sources");

	fc->source = param->string;
	param->string = NULL;
	return 0;
}