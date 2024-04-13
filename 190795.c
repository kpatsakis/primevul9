copyIntoDeepFrameBuffer (const char *& readPtr,
                         char * base,
                         const char* sampleCountBase,
                         ptrdiff_t sampleCountXStride,
                         ptrdiff_t sampleCountYStride,
                         int y, int minX, int maxX,
                         int xOffsetForSampleCount,
                         int yOffsetForSampleCount,
                         int xOffsetForData,
                         int yOffsetForData,
                         ptrdiff_t sampleStride,
                         ptrdiff_t xPointerStride,
                         ptrdiff_t yPointerStride,
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    if(writePtr)
                    {
                        int count = sampleCount(sampleCountBase,
                                                sampleCountXStride,
                                                sampleCountYStride,
                                                x - xOffsetForSampleCount,
                                                y - yOffsetForSampleCount);
                        for (int i = 0; i < count; i++)
                        {
                            *(unsigned int *) writePtr = fillVal;
                            writePtr += sampleStride;
                        }
                    }
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

            {
                half fillVal = half (fillValue);

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    if(writePtr)
                    {                            
                        int count = sampleCount(sampleCountBase,
                                                sampleCountXStride,
                                                sampleCountYStride,
                                                x - xOffsetForSampleCount,
                                                y - yOffsetForSampleCount);
                        for (int i = 0; i < count; i++)
                        {
                            *(half *) writePtr = fillVal;
                           writePtr += sampleStride;
                       }
                    }
                }
            }
            break;

          case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

            {
                float fillVal = float (fillValue);

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    if(writePtr)
                    {
                        int count = sampleCount(sampleCountBase,
                                                sampleCountXStride,
                                                sampleCountYStride,
                                                x - xOffsetForSampleCount,
                                                y - yOffsetForSampleCount);
                        for (int i = 0; i < count; i++)
                        {
                            *(float *) writePtr = fillVal;
                            writePtr += sampleStride;
                        }
                    }
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {
                   
                        for (int i = 0; i < count; i++)
                        {
                            Xdr::read <CharPtrIO> (readPtr, *(unsigned int *) writePtr);
                            writePtr += sampleStride;
                        }
                    }else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<unsigned int>());
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {

                        for (int i = 0; i < count; i++)
                        {
                            half h;
                            Xdr::read <CharPtrIO> (readPtr, h);
                           *(unsigned int *) writePtr = halfToUint (h);
                           writePtr += sampleStride;
                       }
                    }else{
                       Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<half>());
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                                                                        
                    if(writePtr)
                    {
                        for (int i = 0; i < count; i++)
                        {
                            float f;
                            Xdr::read <CharPtrIO> (readPtr, f);
                            *(unsigned int *)writePtr = floatToUint (f);
                            writePtr += sampleStride;
                        } 
                     }else{
                       Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<float>());
                     }
                
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {

                        for (int i = 0; i < count; i++)
                        {
                            unsigned int ui;
                            Xdr::read <CharPtrIO> (readPtr, ui);
                            *(half *) writePtr = uintToHalf (ui);
                            writePtr += sampleStride;
                        }
                    }else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<unsigned int>());
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {
                    
                        for (int i = 0; i < count; i++)
                        {
                            Xdr::read <CharPtrIO> (readPtr, *(half *) writePtr);
                            writePtr += sampleStride;
                        }
                    }else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<half>());
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **) (base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {
                        for (int i = 0; i < count; i++)
                        {
                            float f;
                            Xdr::read <CharPtrIO> (readPtr, f);
                            *(half *) writePtr = floatToHalf (f);
                            writePtr += sampleStride;
                        }
                    }else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<float>());
                    }
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {
                        for (int i = 0; i < count; i++)
                        {
                            unsigned int ui;
                            Xdr::read <CharPtrIO> (readPtr, ui);
                            *(float *) writePtr = float (ui);
                            writePtr += sampleStride;
                        }
                    }else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<unsigned int>());
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {

                        for (int i = 0; i < count; i++)
                        {
                            half h;
                            Xdr::read <CharPtrIO> (readPtr, h);
                            *(float *) writePtr = float (h);
                            writePtr += sampleStride;
                        }
                    
                   }else{
                      Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<half>());
                   }               
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                    if(writePtr)
                    {
                    
                        for (int i = 0; i < count; i++)
                        {
                            Xdr::read <CharPtrIO> (readPtr, *(float *) writePtr);
                            writePtr += sampleStride;
                        }
                    } else{
                        Xdr::skip <CharPtrIO> (readPtr,count*Xdr::size<float>());
                    }      
                    
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                             for (size_t i = 0; i < sizeof (unsigned int); ++i)
                                 writePtr[i] = readPtr[i];

                             readPtr += sizeof (unsigned int);
                             writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(unsigned int)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                        for (int i = 0; i < count; i++)
                        {
                            half h = *(half *) readPtr;
                            *(unsigned int *) writePtr = halfToUint (h);
                            readPtr += sizeof (half);
                            writePtr += sampleStride;
                        }
                    }else{
                        readPtr+=sizeof(half)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                    
                        for (int i = 0; i < count; i++)
                        {
                            float f;

                            for (size_t i = 0; i < sizeof (float); ++i)
                                ((char *)&f)[i] = readPtr[i];

                            *(unsigned int *)writePtr = floatToUint (f);
                            readPtr += sizeof (float);
                            writePtr += sampleStride;
                        }
                    }else{
                        readPtr+=sizeof(float)*count;
                    }
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                             unsigned int ui;
 
                             for (size_t i = 0; i < sizeof (unsigned int); ++i)
                                 ((char *)&ui)[i] = readPtr[i];
  
                             *(half *) writePtr = uintToHalf (ui);
                             readPtr += sizeof (unsigned int);
                             writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(unsigned int)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                             *(half *) writePtr = *(half *)readPtr;
                             readPtr += sizeof (half);
                             writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(half)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                            float f;

                             for (size_t i = 0; i < sizeof (float); ++i)
                                 ((char *)&f)[i] = readPtr[i];

                            *(half *) writePtr = floatToHalf (f);
                            readPtr += sizeof (float);
                            writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(float)*count;
                    }
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

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                              unsigned int ui;
 
                              for (size_t i = 0; i < sizeof (unsigned int); ++i)
                                  ((char *)&ui)[i] = readPtr[i];

                              *(float *) writePtr = float (ui);
                              readPtr += sizeof (unsigned int);
                              writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(unsigned int)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                             half h = *(half *) readPtr;
                             *(float *) writePtr = float (h);
                             readPtr += sizeof (half);
                             writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(half)*count;
                    }
                }
                break;

              case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

                for (int x = minX; x <= maxX; x++)
                {
                    char* writePtr = *(char **)(base+(y-yOffsetForData)*yPointerStride + (x-xOffsetForData)*xPointerStride);
                    
                    int count = sampleCount(sampleCountBase,
                                            sampleCountXStride,
                                            sampleCountYStride,
                                            x - xOffsetForSampleCount,
                                            y - yOffsetForSampleCount);
                                            
                    if(writePtr)
                    {
                         for (int i = 0; i < count; i++)
                         {
                              for (size_t i = 0; i < sizeof (float); ++i)
                                  writePtr[i] = readPtr[i];

                             readPtr += sizeof (float);
                             writePtr += sampleStride;
                         }
                    }else{
                        readPtr+=sizeof(float)*count;
                    }
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