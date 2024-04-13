int OutputValueSampler(CMSREGISTER const cmsUInt16Number In[], CMSREGISTER cmsUInt16Number Out[], CMSREGISTER void* Cargo)
{
    cmsPsSamplerCargo* sc = (cmsPsSamplerCargo*) Cargo;
    cmsUInt32Number i;


    if (sc -> FixWhite) {

        if (In[0] == 0xFFFF) {  // Only in L* = 100, ab = [-8..8]

            if ((In[1] >= 0x7800 && In[1] <= 0x8800) &&
                (In[2] >= 0x7800 && In[2] <= 0x8800)) {

                cmsUInt16Number* Black;
                cmsUInt16Number* White;
                cmsUInt32Number nOutputs;

                if (!_cmsEndPointsBySpace(sc ->ColorSpace, &White, &Black, &nOutputs))
                        return 0;

                for (i=0; i < nOutputs; i++)
                        Out[i] = White[i];
            }


        }
    }


    // Hadle the parenthesis on rows

    if (In[0] != sc ->FirstComponent) {

            if (sc ->FirstComponent != -1) {

                    _cmsIOPrintf(sc ->m, sc ->PostMin);
                    sc ->SecondComponent = -1;
                    _cmsIOPrintf(sc ->m, sc ->PostMaj);
            }

            // Begin block
            _cmsPSActualColumn = 0;

            _cmsIOPrintf(sc ->m, sc ->PreMaj);
            sc ->FirstComponent = In[0];
    }


      if (In[1] != sc ->SecondComponent) {

            if (sc ->SecondComponent != -1) {

                    _cmsIOPrintf(sc ->m, sc ->PostMin);
            }

            _cmsIOPrintf(sc ->m, sc ->PreMin);
            sc ->SecondComponent = In[1];
    }

      // Dump table.

      for (i=0; i < sc -> Pipeline ->Params->nOutputs; i++) {

          cmsUInt16Number wWordOut = Out[i];
          cmsUInt8Number wByteOut;           // Value as byte


          // We always deal with Lab4

          wByteOut = Word2Byte(wWordOut);
          WriteByte(sc -> m, wByteOut);
      }

      return 1;
}