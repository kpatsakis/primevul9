handle_sign_detached_with_file_type(context *ctx, struct pollfd *pollfd, socklen_t size)
{
	handle_sign_helper(ctx, pollfd, size, 0, true);
}