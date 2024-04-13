    void TiffImage::printStructure(std::ostream& out, Exiv2::PrintStructureOption option,int depth)
    {
        if (io_->open() != 0) throw Error(9, io_->path(), strError());
        // Ensure that this is the correct image type
        if (!isTiffType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(14);
            throw Error(15);
        }

        io_->seek(0,BasicIo::beg);

        printTiffStructure(io(),out,option,depth-1);
    }