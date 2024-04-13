getChunkOffsetTableSize(const Header& header)
{
    //
    // if there is a type in the header which indicates the part is not a currently supported type,
    // use the chunkCount attribute
    //


    if(header.hasType()  && !isSupportedType(header.type()))
    {
        if(header.hasChunkCount())
        {
           return header.chunkCount();
        }
        else
        {
           throw IEX_NAMESPACE::ArgExc ("unsupported header type to "
           "get chunk offset table size");
        }
    }

    //
    // part is a known type - ignore the header attribute and compute the chunk size from the header
    //
    if (isTiled(header.type()) == false)
        return getScanlineChunkOffsetTableSize(header);
    else
        return getTiledChunkOffsetTableSize(header);
    
}