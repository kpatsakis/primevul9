                uint64_t readData(int size) const
                {
                    const DataBuf data = Image::io().read(size);
                    assert(data.size_ != 0);

                    uint64_t result = 0;

                    if (size == 1)
                    {}
                    else if (size == 2)
                        result = byteSwap2(data, 0, doSwap_);
                    else if (size == 4)
                        result = byteSwap4(data, 0, doSwap_);
                    else if (size == 8)
                        result = byteSwap8(data, 0, doSwap_);
                    else
                        assert(!"unexpected size");

                    return result;
                }