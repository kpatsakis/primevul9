gopherStateFree(const CommCloseCbParams &params)
{
    GopherStateData *gopherState = (GopherStateData *)params.data;

    if (gopherState == NULL)
        return;

    gopherState->deleteThis("gopherStateFree");
}