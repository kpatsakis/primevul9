static int read_channel_identities (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    unsigned char *idents = wpmd->data;
    int i;

    if (!wpmd->data || !wpmd->byte_length)
        return FALSE;

    for (i = 0; i < wpmd->byte_length; ++i)
        if (!idents [i])
            return FALSE;

    if (!wpc->channel_identities) {
        wpc->channel_identities = (unsigned char *)malloc (wpmd->byte_length + 1);
        memcpy (wpc->channel_identities, wpmd->data, wpmd->byte_length);
        wpc->channel_identities [wpmd->byte_length] = 0;
    }

    return TRUE;
}