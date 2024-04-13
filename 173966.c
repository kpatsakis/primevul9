static void destroy_channel(Encoder *encoder, Channel *channel)
{
    QuicUsrContext *usr = encoder->usr;
    if (channel->correlate_row) {
        usr->free(usr, channel->correlate_row - 1);
    }
    free_family_stat(usr, &channel->family_stat_8bpc);
    free_family_stat(usr, &channel->family_stat_5bpc);
}