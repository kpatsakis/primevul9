bytesPerLineTable (const Header &header,
		   vector<size_t> &bytesPerLine)
{
    const Box2i &dataWindow = header.dataWindow();
    const ChannelList &channels = header.channels();

    bytesPerLine.resize (dataWindow.max.y - dataWindow.min.y + 1);

    for (ChannelList::ConstIterator c = channels.begin();
	 c != channels.end();
	 ++c)
    {
	size_t nBytes = size_t(pixelTypeSize (c.channel().type)) *
		     size_t(dataWindow.max.x - dataWindow.min.x + 1) /
		     size_t(c.channel().xSampling);

	for (int y = dataWindow.min.y, i = 0; y <= dataWindow.max.y; ++y, ++i)
	    if (modp (y, c.channel().ySampling) == 0)
		bytesPerLine[i] += nBytes;
    }

    size_t maxBytesPerLine = 0;

    for (int y = dataWindow.min.y, i = 0; y <= dataWindow.max.y; ++y, ++i)
	if (maxBytesPerLine < bytesPerLine[i])
	    maxBytesPerLine = bytesPerLine[i];

    return maxBytesPerLine;
}