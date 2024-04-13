copyFromDeepFrameBuffer (char *& writePtr,
                         const char * base,
                         char* sampleCountBase,
                         ptrdiff_t sampleCountXStride,
                         ptrdiff_t sampleCountYStride,
                         int y, int xMin, int xMax,
                         int xOffsetForSampleCount,
                         int yOffsetForSampleCount,
                         int xOffsetForData,
                         int yOffsetForData,
                         ptrdiff_t sampleStride,
                         ptrdiff_t dataXStride,
                         ptrdiff_t dataYStride,
                         Compressor::Format format,
                         PixelType type)
{
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

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    Xdr::write <CharPtrIO> (writePtr,
                                            *(const unsigned int *) readPtr);
                    readPtr += sampleStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    Xdr::write <CharPtrIO> (writePtr, *(const half *) readPtr);
                    readPtr += sampleStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;                                   
                                   
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    Xdr::write <CharPtrIO> (writePtr, *(const float *) readPtr);
                    readPtr += sampleStride;
                }
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

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                                   
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;                                                                      
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    for (size_t j = 0; j < sizeof (unsigned int); ++j)
                        *writePtr++ = readPtr[j];

                    readPtr += sampleStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;                                   
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    *(half *) writePtr = *(const half *) readPtr;
                    writePtr += sizeof (half);
                    readPtr += sampleStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            for (int x = xMin; x <= xMax; x++)
            {
                unsigned int count =
                        sampleCount(sampleCountBase,
                                   sampleCountXStride,
                                   sampleCountYStride,
                                   x - xOffsetForSampleCount,
                                   y - yOffsetForSampleCount);
                                   
                const char* ptr = base + (y-yOffsetForData) * dataYStride + (x-xOffsetForData) * dataXStride;                                   
                const char* readPtr = ((const char**) ptr)[0];
                for (unsigned int i = 0; i < count; i++)
                {
                    for (size_t j = 0; j < sizeof (float); ++j)
                        *writePtr++ = readPtr[j];

                    readPtr += sampleStride;
                }
            }
            break;

          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
}