    int JpegBase::advanceToMarker() const
    {
        int c = -1;
        // Skips potential padding between markers
        while ((c=io_->getb()) != 0xff) {
            if (c == EOF) return -1;
        }

        // Markers can start with any number of 0xff
        while ((c=io_->getb()) == 0xff) {
            if (c == EOF) return -2;
        }
        return c;
    }