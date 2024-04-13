gopherTimeout(const CommTimeoutCbParams &io)
{
    GopherStateData *gopherState = static_cast<GopherStateData *>(io.data);
    debugs(10, 4, HERE << io.conn << ": '" << gopherState->entry->url() << "'" );

    gopherState->fwd->fail(new ErrorState(ERR_READ_TIMEOUT, Http::scGatewayTimeout, gopherState->fwd->request));

    if (Comm::IsConnOpen(io.conn))
        io.conn->close();
}