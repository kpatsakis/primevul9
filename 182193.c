void WavInFile::init()
{
    int hdrsOk;

    // assume file stream is already open
    assert(fptr);

    // Read the file headers
    hdrsOk = readWavHeaders();
    if (hdrsOk != 0) 
    {
        // Something didn't match in the wav file headers 
        ST_THROW_RT_ERROR("Input file is corrupt or not a WAV file");
    }

    // sanity check for format parameters
    if ((header.format.channel_number < 1)  || (header.format.channel_number > 9) ||
        (header.format.sample_rate < 4000)  || (header.format.sample_rate > 192000) ||
        (header.format.byte_per_sample < 1) || (header.format.byte_per_sample > 320) ||
        (header.format.bits_per_sample < 8) || (header.format.bits_per_sample > 32))
    {
        // Something didn't match in the wav file headers 
        ST_THROW_RT_ERROR("Error: Illegal wav file header format parameters.");
    }

    dataRead = 0;
}