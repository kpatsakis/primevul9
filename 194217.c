  PropertiesPrivate(File *file, long streamLength) :
    length(0),
    bitrate(0),
    sampleRate(0),
    channels(0),
    version(0),
    bitsPerSample(0),
    file(file),
    streamLength(streamLength) {}