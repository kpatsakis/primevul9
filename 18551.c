handle_sign_attached(context *ctx, struct pollfd *pollfd, socklen_t size)
{
	handle_sign_helper(ctx, pollfd, size, 1, false);
}