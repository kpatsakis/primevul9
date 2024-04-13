bytesPerDeepLineTable (const Header &header,
                       char* base,
                       int xStride,
                       int yStride,
                       vector<size_t> &bytesPerLine)
{
    return bytesPerDeepLineTable(header,
                                 header.dataWindow().min.y,
                                 header.dataWindow().max.y,
                                 base,
                                 xStride,
                                 yStride,
                                 bytesPerLine);
}