convertInPlace (char *& writePtr,
                const char *& readPtr,
		PixelType type,
                size_t numPixels)
{
    switch (type)
    {
      case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
    
        for (size_t j = 0; j < numPixels; ++j)
        {
            union bytesUintOrFloat tmp;
            tmp.b = * reinterpret_cast<const FBytes *>( readPtr );
            Xdr::write <CharPtrIO> (writePtr, tmp.u);
            readPtr += sizeof(unsigned int);
        }
        break;
    
      case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:
    
        for (size_t j = 0; j < numPixels; ++j)
        {               
            Xdr::write <CharPtrIO> (writePtr, *(const half *) readPtr);
            readPtr += sizeof(half);
        }
        break;
    
      case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:
    
        for (size_t j = 0; j < numPixels; ++j)
        {
            union bytesUintOrFloat tmp;
            tmp.b = * reinterpret_cast<const FBytes *>( readPtr );
            Xdr::write <CharPtrIO> (writePtr, tmp.f);
            readPtr += sizeof(float);
        }
        break;
    
      default:
    
        throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
    }
}