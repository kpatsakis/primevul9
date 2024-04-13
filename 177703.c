    WriteMethod TiffParserWorker::encode(
              BasicIo&           io,
        const byte*              pData,
              uint32_t           size,
        const ExifData&          exifData,
        const IptcData&          iptcData,
        const XmpData&           xmpData,
              uint32_t           root,
              FindEncoderFct     findEncoderFct,
              TiffHeaderBase*    pHeader,
              OffsetWriter*      pOffsetWriter
    )
    {
        /*
           1) parse the binary image, if one is provided, and
           2) attempt updating the parsed tree in-place ("non-intrusive writing")
           3) else, create a new tree and write a new TIFF structure ("intrusive
              writing"). If there is a parsed tree, it is only used to access the
              image data in this case.
         */
        assert(pHeader);
        assert(pHeader->byteOrder() != invalidByteOrder);
        WriteMethod writeMethod = wmIntrusive;
        TiffComponent::AutoPtr parsedTree = parse(pData, size, root, pHeader);
        PrimaryGroups primaryGroups;
        findPrimaryGroups(primaryGroups, parsedTree.get());
        if (0 != parsedTree.get()) {
            // Attempt to update existing TIFF components based on metadata entries
            TiffEncoder encoder(exifData,
                                iptcData,
                                xmpData,
                                parsedTree.get(),
                                false,
                                &primaryGroups,
                                pHeader,
                                findEncoderFct);
            parsedTree->accept(encoder);
            if (!encoder.dirty()) writeMethod = wmNonIntrusive;
        }
        if (writeMethod == wmIntrusive) {
            TiffComponent::AutoPtr createdTree = TiffCreator::create(root, ifdIdNotSet);
            if (0 != parsedTree.get()) {
                // Copy image tags from the original image to the composite
                TiffCopier copier(createdTree.get(), root, pHeader, &primaryGroups);
                parsedTree->accept(copier);
            }
            // Add entries from metadata to composite
            TiffEncoder encoder(exifData,
                                iptcData,
                                xmpData,
                                createdTree.get(),
                                parsedTree.get() == 0,
                                &primaryGroups,
                                pHeader,
                                findEncoderFct);
            encoder.add(createdTree.get(), parsedTree.get(), root);
            // Write binary representation from the composite tree
            DataBuf header = pHeader->write();
            BasicIo::AutoPtr tempIo(io.temporary()); // may throw
            assert(tempIo.get() != 0);
            IoWrapper ioWrapper(*tempIo, header.pData_, header.size_, pOffsetWriter);
            uint32_t imageIdx(uint32_t(-1));
            createdTree->write(ioWrapper,
                               pHeader->byteOrder(),
                               header.size_,
                               uint32_t(-1),
                               uint32_t(-1),
                               imageIdx);
            if (pOffsetWriter) pOffsetWriter->writeOffsets(*tempIo);
            io.transfer(*tempIo); // may throw
#ifndef SUPPRESS_WARNINGS
            EXV_INFO << "Write strategy: Intrusive\n";
#endif
        }
#ifndef SUPPRESS_WARNINGS
        else {
            EXV_INFO << "Write strategy: Non-intrusive\n";
        }
#endif
        return writeMethod;
    } // TiffParserWorker::encode