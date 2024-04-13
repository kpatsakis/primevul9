      // Insert code instructions for processing scalars.
      unsigned int scalar() { // Insert new scalar in memory.
        if (mempos>=mem._width) { mem.resize(-200,1,1,1,0); memtype.resize(mem._width,1,1,1,0); }
        return mempos++;