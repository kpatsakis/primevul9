    bool isTiffImageTag(uint16_t tag, IfdId group)
    {
        //! List of TIFF image tags
        static const TiffImgTagStruct tiffImageTags[] = {
            { 0x00fe, ifd0Id }, // Exif.Image.NewSubfileType
            { 0x00ff, ifd0Id }, // Exif.Image.SubfileType
            { 0x0100, ifd0Id }, // Exif.Image.ImageWidth
            { 0x0101, ifd0Id }, // Exif.Image.ImageLength
            { 0x0102, ifd0Id }, // Exif.Image.BitsPerSample
            { 0x0103, ifd0Id }, // Exif.Image.Compression
            { 0x0106, ifd0Id }, // Exif.Image.PhotometricInterpretation
            { 0x010a, ifd0Id }, // Exif.Image.FillOrder
            { 0x0111, ifd0Id }, // Exif.Image.StripOffsets
            { 0x0115, ifd0Id }, // Exif.Image.SamplesPerPixel
            { 0x0116, ifd0Id }, // Exif.Image.RowsPerStrip
            { 0x0117, ifd0Id }, // Exif.Image.StripByteCounts
            { 0x011a, ifd0Id }, // Exif.Image.XResolution
            { 0x011b, ifd0Id }, // Exif.Image.YResolution
            { 0x011c, ifd0Id }, // Exif.Image.PlanarConfiguration
            { 0x0122, ifd0Id }, // Exif.Image.GrayResponseUnit
            { 0x0123, ifd0Id }, // Exif.Image.GrayResponseCurve
            { 0x0124, ifd0Id }, // Exif.Image.T4Options
            { 0x0125, ifd0Id }, // Exif.Image.T6Options
            { 0x0128, ifd0Id }, // Exif.Image.ResolutionUnit
            { 0x0129, ifd0Id }, // Exif.Image.PageNumber
            { 0x012d, ifd0Id }, // Exif.Image.TransferFunction
            { 0x013d, ifd0Id }, // Exif.Image.Predictor
            { 0x013e, ifd0Id }, // Exif.Image.WhitePoint
            { 0x013f, ifd0Id }, // Exif.Image.PrimaryChromaticities
            { 0x0140, ifd0Id }, // Exif.Image.ColorMap
            { 0x0141, ifd0Id }, // Exif.Image.HalftoneHints
            { 0x0142, ifd0Id }, // Exif.Image.TileWidth
            { 0x0143, ifd0Id }, // Exif.Image.TileLength
            { 0x0144, ifd0Id }, // Exif.Image.TileOffsets
            { 0x0145, ifd0Id }, // Exif.Image.TileByteCounts
            { 0x014c, ifd0Id }, // Exif.Image.InkSet
            { 0x014d, ifd0Id }, // Exif.Image.InkNames
            { 0x014e, ifd0Id }, // Exif.Image.NumberOfInks
            { 0x0150, ifd0Id }, // Exif.Image.DotRange
            { 0x0151, ifd0Id }, // Exif.Image.TargetPrinter
            { 0x0152, ifd0Id }, // Exif.Image.ExtraSamples
            { 0x0153, ifd0Id }, // Exif.Image.SampleFormat
            { 0x0154, ifd0Id }, // Exif.Image.SMinSampleValue
            { 0x0155, ifd0Id }, // Exif.Image.SMaxSampleValue
            { 0x0156, ifd0Id }, // Exif.Image.TransferRange
            { 0x0157, ifd0Id }, // Exif.Image.ClipPath
            { 0x0158, ifd0Id }, // Exif.Image.XClipPathUnits
            { 0x0159, ifd0Id }, // Exif.Image.YClipPathUnits
            { 0x015a, ifd0Id }, // Exif.Image.Indexed
            { 0x015b, ifd0Id }, // Exif.Image.JPEGTables
            { 0x0200, ifd0Id }, // Exif.Image.JPEGProc
            { 0x0201, ifd0Id }, // Exif.Image.JPEGInterchangeFormat
            { 0x0202, ifd0Id }, // Exif.Image.JPEGInterchangeFormatLength
            { 0x0203, ifd0Id }, // Exif.Image.JPEGRestartInterval
            { 0x0205, ifd0Id }, // Exif.Image.JPEGLosslessPredictors
            { 0x0206, ifd0Id }, // Exif.Image.JPEGPointTransforms
            { 0x0207, ifd0Id }, // Exif.Image.JPEGQTables
            { 0x0208, ifd0Id }, // Exif.Image.JPEGDCTables
            { 0x0209, ifd0Id }, // Exif.Image.JPEGACTables
            { 0x0211, ifd0Id }, // Exif.Image.YCbCrCoefficients
            { 0x0212, ifd0Id }, // Exif.Image.YCbCrSubSampling
            { 0x0213, ifd0Id }, // Exif.Image.YCbCrPositioning
            { 0x0214, ifd0Id }, // Exif.Image.ReferenceBlackWhite
            { 0x828d, ifd0Id }, // Exif.Image.CFARepeatPatternDim
            { 0x828e, ifd0Id }, // Exif.Image.CFAPattern
            { 0x8773, ifd0Id }, // Exif.Image.InterColorProfile
            { 0x8824, ifd0Id }, // Exif.Image.SpectralSensitivity
            { 0x8828, ifd0Id }, // Exif.Image.OECF
            { 0x9102, ifd0Id }, // Exif.Image.CompressedBitsPerPixel
            { 0x9217, ifd0Id }, // Exif.Image.SensingMethod
        };

        // If tag, group is one of the image tags listed above -> bingo!
        if (find(tiffImageTags, TiffImgTagStruct::Key(tag, group))) {
#ifdef DEBUG
            ExifKey key(tag, groupName(group));
            std::cerr << "Image tag: " << key << " (3)\n";
#endif
            return true;
        }
#ifdef DEBUG
        std::cerr << "Not an image tag: " << tag << " (4)\n";
#endif
        return false;
    }