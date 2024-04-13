copyFromFrameBuffer (char *& writePtr,
		     const char *& readPtr,
                     const char * endPtr,
		     size_t xStride,
                     Compressor::Format format,
		     PixelType type)
{
    char * localWritePtr = writePtr;
    const char * localReadPtr = readPtr;
    //
    // Copy a horizontal row of pixels from a frame
    // buffer to an output file's line or tile buffer.
    //

    if (format == Compressor::XDR)
    {
        //
        // The the line or tile buffer is in XDR format.
        //

        switch (type)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

            while (localReadPtr <= endPtr)
            {
                Xdr::write <CharPtrIO> (localWritePtr,
                                        *(const unsigned int *) localReadPtr);
                localReadPtr += xStride;
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            while (localReadPtr <= endPtr)
            {
                Xdr::write <CharPtrIO> (localWritePtr, *(const half *) localReadPtr);
                localReadPtr += xStride;
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            while (localReadPtr <= endPtr)
            {
                Xdr::write <CharPtrIO> (localWritePtr, *(const float *) localReadPtr);
                localReadPtr += xStride;
            }
            break;

          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
    else
    {
        //
        // The the line or tile buffer is in NATIVE format.
        //

        switch (type)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

            while (localReadPtr <= endPtr)
            {
                for (size_t i = 0; i < sizeof (unsigned int); ++i)
                    *localWritePtr++ = localReadPtr[i];

                localReadPtr += xStride;
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            while (localReadPtr <= endPtr)
            {
                *(half *) localWritePtr = *(const half *) localReadPtr;
                localWritePtr += sizeof (half);
                localReadPtr += xStride;
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            while (localReadPtr <= endPtr)
            {
                for (size_t i = 0; i < sizeof (float); ++i)
                    *localWritePtr++ = localReadPtr[i];

                localReadPtr += xStride;
            }
            break;
            
          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }

    writePtr = localWritePtr;
    readPtr = localReadPtr;
}