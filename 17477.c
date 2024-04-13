void DL_Dxf::writeDimStyleOverrides(DL_WriterA& dw,
                             const DL_DimensionData& data) {

    if (version==DL_VERSION_2000) {
        dw.dxfString(1001, "ACAD");
        dw.dxfString(1000, "DSTYLE");
        dw.dxfString(1002, "{");
        if (data.type&128) {
            // custom text position:
            dw.dxfInt(1070, 279);
            dw.dxfInt(1070, 2);
        }
        dw.dxfInt(1070, 144);
        dw.dxfReal(1040, data.linearFactor);
        dw.dxfInt(1070,40);
        dw.dxfReal(1040, data.dimScale);
        dw.dxfString(1002, "}");
    }
}