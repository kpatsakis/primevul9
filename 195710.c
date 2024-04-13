int32_t cli_bcapi_memstr(struct cli_bc_ctx *ctx, const uint8_t* h, int32_t hs,
			 const uint8_t*n, int32_t ns)
{
    const uint8_t *s;
    if (!h || !n || hs < 0 || ns < 0) {
	API_MISUSE();
	return -1;
    }
    cli_event_fastdata(EV, BCEV_MEM_1, h, hs);
    cli_event_fastdata(EV, BCEV_MEM_2, n, ns);
    s = (const uint8_t*) cli_memstr((const char*)h, hs, (const char*)n, ns);
    if (!s)
	return -1;
    return s - h;
}