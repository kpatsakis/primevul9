fillChannelWithZeroes (char *& writePtr,
		       Compressor::Format format,
		       PixelType type,
		       size_t xSize)
{
    if (format == Compressor::XDR)
    {
        //
        // Fill with data in XDR format.
        //

        switch (type)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

            for (size_t j = 0; j < xSize; ++j)
                Xdr::write <CharPtrIO> (writePtr, (unsigned int) 0);

            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            for (size_t j = 0; j < xSize; ++j)
                Xdr::write <CharPtrIO> (writePtr, (half) 0);

            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            for (size_t j = 0; j < xSize; ++j)
                Xdr::write <CharPtrIO> (writePtr, (float) 0);

            break;
            
          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
    else
    {
        //
        // Fill with data in NATIVE format.
        //

        switch (type)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

            for (size_t j = 0; j < xSize; ++j)
            {
                static const unsigned int ui = 0;

                for (size_t i = 0; i < sizeof (ui); ++i)
                    *writePtr++ = ((char *) &ui)[i];
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            for (size_t j = 0; j < xSize; ++j)
            {
                *(half *) writePtr = half (0);
                writePtr += sizeof (half);
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            for (size_t j = 0; j < xSize; ++j)
            {
                static const float f = 0;

                for (size_t i = 0; i < sizeof (f); ++i)
                    *writePtr++ = ((char *) &f)[i];
            }
            break;
            
          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
}