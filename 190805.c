skipChannel (const char *& readPtr,
             PixelType typeInFile,
	     size_t xSize)
{
    switch (typeInFile)
    {
      case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
        
        Xdr::skip <CharPtrIO> (readPtr, Xdr::size <unsigned int> () * xSize);
        break;

      case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

        Xdr::skip <CharPtrIO> (readPtr, Xdr::size <half> () * xSize);
        break;

      case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

        Xdr::skip <CharPtrIO> (readPtr, Xdr::size <float> () * xSize);
        break;

      default:

        throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
    }
}