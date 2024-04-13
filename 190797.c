getScanlineChunkOffsetTableSize(const Header& header)
{
    const Box2i &dataWindow = header.dataWindow();


    //
    // use int64_t types to prevent overflow in lineOffsetSize for images with
    // extremely high dataWindows
    //
    int64_t linesInBuffer = numLinesInBuffer ( header.compression() );

    int64_t lineOffsetSize = (static_cast <int64_t>(dataWindow.max.y) - static_cast <int64_t>(dataWindow.min.y) +
                          linesInBuffer) / linesInBuffer;

    return static_cast <int>(lineOffsetSize);
}