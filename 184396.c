static void unreorder_channels (int32_t *data, unsigned char *order, int num_chans, int num_samples)
{
    int32_t reorder_buffer [16], *temp = reorder_buffer;

    if (num_chans > 16)
        temp = malloc (num_chans * sizeof (*data));

    while (num_samples--) {
        int chan;

        for (chan = 0; chan < num_chans; ++chan)
            temp [chan] = data [order[chan]];

        memcpy (data, temp, num_chans * sizeof (*data));
        data += num_chans;
    }

    if (num_chans > 16)
        free (temp);
}