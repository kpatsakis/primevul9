DwaCompressor::LossyDctDecoderBase::execute ()
{
    size_t numComp        = _rowPtrs.size();
    int lastNonZero    = 0;
    int numBlocksX     = (int) ceil ((float)_width  / 8.0f);
    int numBlocksY     = (int) ceil ((float)_height / 8.0f);
    int leftoverX      = _width  - (numBlocksX-1) * 8;
    int leftoverY      = _height - (numBlocksY-1) * 8;

    int numFullBlocksX = (int)floor ((float)_width / 8.0f);

    unsigned short tmpShortNative = 0;
    unsigned short tmpShortXdr    = 0;
    const char *tmpConstCharPtr   = 0;

    unsigned short                    *currAcComp = (unsigned short *)_packedAc;
    std::vector<unsigned short *>      currDcComp (_rowPtrs.size());
    std::vector<SimdAlignedBuffer64us> halfZigBlock (_rowPtrs.size());

    if (_type.size() != _rowPtrs.size())
        throw IEX_NAMESPACE::BaseExc ("Row pointers and types mismatch in count");

    if ((_rowPtrs.size() != 3) && (_rowPtrs.size() != 1))
        throw IEX_NAMESPACE::NoImplExc ("Only 1 and 3 channel encoding is supported");

    _dctData.resize(numComp);

    //
    // Allocate a temp aligned buffer to hold a rows worth of full 
    // 8x8 half-float blocks
    //

    unsigned char *rowBlockHandle = new unsigned char
        [numComp * numBlocksX * 64 * sizeof(unsigned short) + _SSE_ALIGNMENT];

    unsigned short *rowBlock[3];

    rowBlock[0] = (unsigned short*)rowBlockHandle;

    for (int i = 0; i < _SSE_ALIGNMENT; ++i)
    {
        if ((reinterpret_cast<uintptr_t>(rowBlockHandle + i) & _SSE_ALIGNMENT_MASK) == 0)
            rowBlock[0] = (unsigned short *)(rowBlockHandle + i);
    }

    for (size_t comp = 1; comp < numComp; ++comp)
        rowBlock[comp] = rowBlock[comp - 1] + numBlocksX * 64;
 
    //
    // Pack DC components together by common plane, so we can get 
    // a little more out of differencing them. We'll always have 
    // one component per block, so we can computed offsets.
    //

    currDcComp[0] = (unsigned short *)_packedDc;

    for (size_t comp = 1; comp < numComp; ++comp)
        currDcComp[comp] = currDcComp[comp - 1] + numBlocksX * numBlocksY;

    for (int blocky = 0; blocky < numBlocksY; ++blocky)
    {
        int maxY = 8;

        if (blocky == numBlocksY-1)
            maxY = leftoverY;

        int maxX = 8;

        for (int blockx = 0; blockx < numBlocksX; ++blockx)
        {
            if (blockx == numBlocksX-1)
                maxX = leftoverX;

            //
            // If we can detect that the block is constant values
            // (all components only have DC values, and all AC is 0),
            // we can do everything only on 1 value, instead of all
            // 64. 
            //
            // This won't really help for regular images, but it is
            // meant more for layers with large swaths of black 
            //

            bool blockIsConstant = true;

            for (size_t comp = 0; comp < numComp; ++comp)
            {

                //
                // DC component is stored separately
                //

                #ifdef IMF_HAVE_SSE2
                    {
                        __m128i *dst = (__m128i*)halfZigBlock[comp]._buffer;

                        dst[7] = _mm_setzero_si128();
                        dst[6] = _mm_setzero_si128();
                        dst[5] = _mm_setzero_si128();
                        dst[4] = _mm_setzero_si128();
                        dst[3] = _mm_setzero_si128();
                        dst[2] = _mm_setzero_si128();
                        dst[1] = _mm_setzero_si128();
                        dst[0] = _mm_insert_epi16
                            (_mm_setzero_si128(), *currDcComp[comp]++, 0);
                    }
                #else  /* IMF_HAVE_SSE2 */

                    memset (halfZigBlock[comp]._buffer, 0, 64 * 2);
                    halfZigBlock[comp]._buffer[0] = *currDcComp[comp]++;

                #endif /* IMF_HAVE_SSE2 */

                _packedDcCount++;
                
                //
                // UnRLE the AC. This will modify currAcComp
                //

                lastNonZero = unRleAc (currAcComp, halfZigBlock[comp]._buffer);

                //
                // Convert from XDR to NATIVE
                //

                if (!_isNativeXdr)
                {
                    for (int i = 0; i < 64; ++i)
                    {
                        tmpShortXdr      = halfZigBlock[comp]._buffer[i];
                        tmpConstCharPtr  = (const char *)&tmpShortXdr;

                        Xdr::read<CharPtrIO> (tmpConstCharPtr, tmpShortNative);

                        halfZigBlock[comp]._buffer[i] = tmpShortNative;
                    }
                }

                if (lastNonZero == 0)
                {
                    //
                    // DC only case - AC components are all 0   
                    //

                    half h;

                    h.setBits (halfZigBlock[comp]._buffer[0]);
                    _dctData[comp]._buffer[0] = (float)h;

                    dctInverse8x8DcOnly (_dctData[comp]._buffer);
                }
                else
                {
                    //
                    // We have some AC components that are non-zero. 
                    // Can't use the 'constant block' optimization
                    //

                    blockIsConstant = false;

                    //
                    // Un-Zig zag 
                    //

                    (*fromHalfZigZag)
                        (halfZigBlock[comp]._buffer, _dctData[comp]._buffer);

                    //
                    // Zig-Zag indices in normal layout are as follows:
                    //
                    // 0   1   5   6   14  15  27  28
                    // 2   4   7   13  16  26  29  42
                    // 3   8   12  17  25  30  41  43
                    // 9   11  18  24  31  40  44  53
                    // 10  19  23  32  39  45  52  54
                    // 20  22  33  38  46  51  55  60
                    // 21  34  37  47  50  56  59  61
                    // 35  36  48  49  57  58  62  63
                    //
                    // If lastNonZero is less than the first item on
                    // each row, we know that the whole row is zero and 
                    // can be skipped in the row-oriented part of the
                    // iDCT. 
                    //
                    // The unrolled logic here is:
                    //
                    //    if lastNonZero < rowStartIdx[i],
                    //    zeroedRows = rowsEmpty[i]
                    //
                    // where:
                    //
                    //    const int rowStartIdx[] = {2, 3, 9, 10, 20, 21, 35};
                    //    const int rowsEmpty[]   = {7, 6, 5,  4,  3,  2,  1};
                    //

                    if (lastNonZero < 2)
                        dctInverse8x8_7(_dctData[comp]._buffer);
                    else if (lastNonZero < 3)
                        dctInverse8x8_6(_dctData[comp]._buffer);
                    else if (lastNonZero < 9)
                        dctInverse8x8_5(_dctData[comp]._buffer);
                    else if (lastNonZero < 10)
                        dctInverse8x8_4(_dctData[comp]._buffer);
                    else if (lastNonZero < 20)
                        dctInverse8x8_3(_dctData[comp]._buffer);
                    else if (lastNonZero < 21)
                        dctInverse8x8_2(_dctData[comp]._buffer);
                    else if (lastNonZero < 35)
                        dctInverse8x8_1(_dctData[comp]._buffer);
                    else
                        dctInverse8x8_0(_dctData[comp]._buffer);
                }
            }

            //
            // Perform the CSC
            //

            if (numComp == 3)
            {
                if (!blockIsConstant)
                {
                    csc709Inverse64 (_dctData[0]._buffer, 
                                     _dctData[1]._buffer, 
                                     _dctData[2]._buffer);

                }
                else
                {
                    csc709Inverse (_dctData[0]._buffer[0], 
                                   _dctData[1]._buffer[0], 
                                   _dctData[2]._buffer[0]);
                }
            }

            //
            // Float -> Half conversion. 
            //
            // If the block has a constant value, just convert the first pixel.
            //

            for (size_t comp = 0; comp < numComp; ++comp)
            {
                if (!blockIsConstant)
                {
                    (*convertFloatToHalf64)
                        (&rowBlock[comp][blockx*64], _dctData[comp]._buffer);
                }
                else
                {
                    #ifdef IMF_HAVE_SSE2

                        __m128i *dst = (__m128i*)&rowBlock[comp][blockx*64];

                        dst[0] = _mm_set1_epi16
                            (((half)_dctData[comp]._buffer[0]).bits());

                        dst[1] = dst[0];
                        dst[2] = dst[0];
                        dst[3] = dst[0];
                        dst[4] = dst[0];
                        dst[5] = dst[0];
                        dst[6] = dst[0];
                        dst[7] = dst[0];

                    #else  /* IMF_HAVE_SSE2 */

                        unsigned short *dst = &rowBlock[comp][blockx*64];

                        dst[0] = ((half)_dctData[comp]._buffer[0]).bits();

                        for (int i = 1; i < 64; ++i)
                        {
                            dst[i] = dst[0];
                        }

                    #endif /* IMF_HAVE_SSE2 */
                } // blockIsConstant
            } // comp
        } // blockx

        //
        // At this point, we have half-float nonlinear value blocked
        // in rowBlock[][]. We need to unblock the data, transfer
        // back to linear, and write the results in the _rowPtrs[].
        //
        // There is a fast-path for aligned rows, which helps
        // things a little. Since this fast path is only valid
        // for full 8-element wide blocks, the partial x blocks
        // are broken into a separate loop below.
        //
        // At the moment, the fast path requires:
        //   * sse support
        //   * aligned row pointers
        //   * full 8-element wide blocks
        //

        for (size_t comp = 0; comp < numComp; ++comp)
        {
            //
            // Test if we can use the fast path
            //

        #ifdef IMF_HAVE_SSE2

            bool fastPath = true;

            for (int y = 8 * blocky; y < 8 * blocky + maxY; ++y)
            {
                if (reinterpret_cast<uintptr_t>(_rowPtrs[comp][y]) & _SSE_ALIGNMENT_MASK)
                    fastPath = false;
            }

            if (fastPath)
            {
                //
                // Handle all the full X blocks, in a fast path with sse2 and
                // aligned row pointers
                //

                for (int y=8*blocky; y<8*blocky+maxY; ++y)
                {
                    __m128i *dst = (__m128i *)_rowPtrs[comp][y];
                    __m128i *src = (__m128i *)&rowBlock[comp][(y & 0x7) * 8];


                    for (int blockx = 0; blockx < numFullBlocksX; ++blockx)
                    {
                        //
                        // These may need some twiddling.
                        // Run with multiples of 8
                        //

                        _mm_prefetch ((char *)(src + 16), _MM_HINT_NTA); 

                        unsigned short i0  = _mm_extract_epi16 (*src, 0);
                        unsigned short i1  = _mm_extract_epi16 (*src, 1);
                        unsigned short i2  = _mm_extract_epi16 (*src, 2);
                        unsigned short i3  = _mm_extract_epi16 (*src, 3);

                        unsigned short i4  = _mm_extract_epi16 (*src, 4);
                        unsigned short i5  = _mm_extract_epi16 (*src, 5);
                        unsigned short i6  = _mm_extract_epi16 (*src, 6);
                        unsigned short i7  = _mm_extract_epi16 (*src, 7);

                        i0 = _toLinear[i0];
                        i1 = _toLinear[i1];
                        i2 = _toLinear[i2];
                        i3 = _toLinear[i3];

                        i4 = _toLinear[i4];
                        i5 = _toLinear[i5];
                        i6 = _toLinear[i6];
                        i7 = _toLinear[i7];

                        *dst = _mm_insert_epi16 (_mm_setzero_si128(), i0, 0);
                        *dst = _mm_insert_epi16 (*dst, i1, 1);
                        *dst = _mm_insert_epi16 (*dst, i2, 2);
                        *dst = _mm_insert_epi16 (*dst, i3, 3);

                        *dst = _mm_insert_epi16 (*dst, i4, 4);
                        *dst = _mm_insert_epi16 (*dst, i5, 5);
                        *dst = _mm_insert_epi16 (*dst, i6, 6);
                        *dst = _mm_insert_epi16 (*dst, i7, 7);

                        src += 8;
                        dst++;
                    }
                }
            }
            else
            {

        #endif /* IMF_HAVE_SSE2 */

                //
                // Basic scalar kinda slow path for handling the full X blocks
                //

                for (int y = 8 * blocky; y < 8 * blocky + maxY; ++y)
                {
                    unsigned short *dst = (unsigned short *)_rowPtrs[comp][y];

                    for (int blockx = 0; blockx < numFullBlocksX; ++blockx)
                    {
                        unsigned short *src =
                            &rowBlock[comp][blockx * 64 + ((y & 0x7) * 8)];

                        dst[0] = _toLinear[src[0]];
                        dst[1] = _toLinear[src[1]];
                        dst[2] = _toLinear[src[2]];
                        dst[3] = _toLinear[src[3]];

                        dst[4] = _toLinear[src[4]];
                        dst[5] = _toLinear[src[5]];
                        dst[6] = _toLinear[src[6]];
                        dst[7] = _toLinear[src[7]];

                        dst += 8;
                    }
                }

        #ifdef IMF_HAVE_SSE2

            }

        #endif /* IMF_HAVE_SSE2 */

            //
            // If we have partial X blocks, deal with all those now
            // Since this should be minimal work, there currently
            // is only one path that should work for everyone.
            //

            if (numFullBlocksX != numBlocksX)
            {
                for (int y = 8 * blocky; y < 8 * blocky + maxY; ++y)
                {
                    unsigned short *src = (unsigned short *)
                        &rowBlock[comp][numFullBlocksX * 64 + ((y & 0x7) * 8)];

                    unsigned short *dst = (unsigned short *)_rowPtrs[comp][y];

                    dst += 8 * numFullBlocksX;

                    for (int x = 0; x < maxX; ++x)
                    {
                        *dst++ = _toLinear[*src++];
                    }
                }
            }
        } // comp
    } // blocky

    //
    // Walk over all the channels that are of type FLOAT.
    // Convert from HALF XDR back to FLOAT XDR.
    //

    for (size_t chan = 0; chan < numComp; ++chan)
    {

        if (_type[chan] != FLOAT)
            continue;

        std::vector<unsigned short> halfXdr (_width);

        for (int y=0; y<_height; ++y)
        {
            char *floatXdrPtr = _rowPtrs[chan][y];

            memcpy(&halfXdr[0], floatXdrPtr, _width*sizeof(unsigned short));

            const char *halfXdrPtr = (const char *)(&halfXdr[0]);

            for (int x=0; x<_width; ++x)
            {
                half tmpHalf;

                Xdr::read<CharPtrIO> (halfXdrPtr, tmpHalf);
                Xdr::write<CharPtrIO> (floatXdrPtr, (float)tmpHalf);

                // 
                // Xdr::write and Xdr::read will advance the ptrs
                //
            }
        }
    }

    delete[] rowBlockHandle;
}