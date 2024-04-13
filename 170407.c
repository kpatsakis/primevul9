static bool arg_type_may_be_null(enum bpf_arg_type type)
{
	return type == ARG_PTR_TO_MAP_VALUE_OR_NULL ||
	       type == ARG_PTR_TO_MEM_OR_NULL ||
	       type == ARG_PTR_TO_CTX_OR_NULL ||
	       type == ARG_PTR_TO_SOCKET_OR_NULL ||
	       type == ARG_PTR_TO_ALLOC_MEM_OR_NULL;
}