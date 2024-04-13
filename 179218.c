    Jp2Image::Jp2Image(BasicIo::AutoPtr io, bool create)
            : Image(ImageType::jp2, mdExif | mdIptc | mdXmp, io)
    {
        if (create)
        {
            if (io_->open() == 0)
            {
#ifdef EXIV2_DEBUG_MESSAGES
                std::cerr << "Exiv2::Jp2Image:: Creating JPEG2000 image to memory" << std::endl;
#endif
                IoCloser closer(*io_);
                if (io_->write(Jp2Blank, sizeof(Jp2Blank)) != sizeof(Jp2Blank))
                {
#ifdef EXIV2_DEBUG_MESSAGES
                    std::cerr << "Exiv2::Jp2Image:: Failed to create JPEG2000 image on memory" << std::endl;
#endif
                }
            }
        }
    } // Jp2Image::Jp2Image