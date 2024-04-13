    virtual UClassID getDynamicClassID(void) const {
        static char classID = 0;
        return (UClassID)&classID;
    }