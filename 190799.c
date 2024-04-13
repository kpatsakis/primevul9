copyIntoFrameBuffer (const char *& readPtr,
		     char * writePtr,
		     char * endPtr,
                     size_t xStride,
		     bool fill,
		     double fillValue,
                     Compressor::Format format,
                     PixelType typeInFrameBuffer,
                     PixelType typeInFile)
{
    //
    // Copy a horizontal row of pixels from an input
    // file's line or tile buffer to a frame buffer.
    //

    if (fill)
    {
        //
        // The file contains no data for this channel.
        // Store a default value in the frame buffer.
        //

        switch (typeInFrameBuffer)
        {
	  case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
            
            {
                unsigned int fillVal = (unsigned int) (fillValue);

                while (writePtr <= endPtr)
                {
                    *(unsigned int *) writePtr = fillVal;
                    writePtr += xStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            {
                half fillVal = half (fillValue);

                while (writePtr <= endPtr)
                {
                    *(half *) writePtr = fillVal;
                    writePtr += xStride;
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            {
                float fillVal = float (fillValue);

                while (writePtr <= endPtr)
                {
                    *(float *) writePtr = fillVal;
                    writePtr += xStride;
                }
            }
            break;

          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
    else if (format == Compressor::XDR)
    {
        //
        // The the line or tile buffer is in XDR format.
        //
        // Convert the pixels from the file's machine-
        // independent representation, and store the
        // results in the frame buffer.
        //

        switch (typeInFrameBuffer)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
    
            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    Xdr::read <CharPtrIO> (readPtr, *(unsigned int *) writePtr);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                while (writePtr <= endPtr)
                {
                    half h;
                    Xdr::read <CharPtrIO> (readPtr, h);
                    *(unsigned int *) writePtr = halfToUint (h);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    float f;
                    Xdr::read <CharPtrIO> (readPtr, f);
                    *(unsigned int *)writePtr = floatToUint (f);
                    writePtr += xStride;
                }
                break;
                
              default:                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    unsigned int ui;
                    Xdr::read <CharPtrIO> (readPtr, ui);
                    *(half *) writePtr = uintToHalf (ui);
                    writePtr += xStride;
                }
                break;
                
              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                while (writePtr <= endPtr)
                {
                    Xdr::read <CharPtrIO> (readPtr, *(half *) writePtr);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    float f;
                    Xdr::read <CharPtrIO> (readPtr, f);
                    *(half *) writePtr = floatToHalf (f);
                    writePtr += xStride;
                }
                break;
              default:
                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    unsigned int ui;
                    Xdr::read <CharPtrIO> (readPtr, ui);
                    *(float *) writePtr = float (ui);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                while (writePtr <= endPtr)
                {
                    half h;
                    Xdr::read <CharPtrIO> (readPtr, h);
                    *(float *) writePtr = float (h);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    Xdr::read <CharPtrIO> (readPtr, *(float *) writePtr);
                    writePtr += xStride;
                }
                break;
              default:
                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
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
        // Copy the results into the frame buffer.
        //

        switch (typeInFrameBuffer)
        {
          case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
    
            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    for (size_t i = 0; i < sizeof (unsigned int); ++i)
                        writePtr[i] = readPtr[i];

                    readPtr += sizeof (unsigned int);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                while (writePtr <= endPtr)
                {
                    half h = *(half *) readPtr;
                    *(unsigned int *) writePtr = halfToUint (h);
                    readPtr += sizeof (half);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    float f;

                    for (size_t i = 0; i < sizeof (float); ++i)
                        ((char *)&f)[i] = readPtr[i];

                    *(unsigned int *)writePtr = floatToUint (f);
                    readPtr += sizeof (float);
                    writePtr += xStride;
                }
                break;
                
              default:
                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    unsigned int ui;

                    for (size_t i = 0; i < sizeof (unsigned int); ++i)
                        ((char *)&ui)[i] = readPtr[i];

                    *(half *) writePtr = uintToHalf (ui);
                    readPtr += sizeof (unsigned int);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                // If we're tightly packed, just memcpy
                if (xStride == sizeof(half)) {
                    int numBytes = endPtr-writePtr+sizeof(half);
                    memcpy(writePtr, readPtr, numBytes);
                    readPtr  += numBytes;
                    writePtr += numBytes;                    
                } else {
                    while (writePtr <= endPtr)
                    {
                        *(half *) writePtr = *(half *)readPtr;
                        readPtr += sizeof (half);
                        writePtr += xStride;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    float f;

                    for (size_t i = 0; i < sizeof (float); ++i)
                        ((char *)&f)[i] = readPtr[i];

                    *(half *) writePtr = floatToHalf (f);
                    readPtr += sizeof (float);
                    writePtr += xStride;
                }
                break;
              default:
                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            switch (typeInFile)
            {
              case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:

                while (writePtr <= endPtr)
                {
                    unsigned int ui;

                    for (size_t i = 0; i < sizeof (unsigned int); ++i)
                        ((char *)&ui)[i] = readPtr[i];

                    *(float *) writePtr = float (ui);
                    readPtr += sizeof (unsigned int);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                while (writePtr <= endPtr)
                {
                    half h = *(half *) readPtr;
                    *(float *) writePtr = float (h);
                    readPtr += sizeof (half);
                    writePtr += xStride;
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                while (writePtr <= endPtr)
                {
                    for (size_t i = 0; i < sizeof (float); ++i)
                        writePtr[i] = readPtr[i];

                    readPtr += sizeof (float);
                    writePtr += xStride;
                }
                break;
              default:
                  
                  throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
            }
            break;

          default:

            throw IEX_NAMESPACE::ArgExc ("Unknown pixel data type.");
        }
    }
}