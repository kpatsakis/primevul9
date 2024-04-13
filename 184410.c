int WavpackGetMD5Sum (WavpackContext *wpc, unsigned char data [16])
{
    if (wpc->config.flags & CONFIG_MD5_CHECKSUM) {
        if (!wpc->config.md5_read && wpc->reader->can_seek (wpc->wv_in))
            seek_eof_information (wpc, NULL, FALSE);

        if (wpc->config.md5_read) {
            memcpy (data, wpc->config.md5_checksum, 16);
            return TRUE;
        }
    }

    return FALSE;
}