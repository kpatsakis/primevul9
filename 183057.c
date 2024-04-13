cmsFloat64Number* GetPtrToMatrix(const cmsStage* mpe)
{
    _cmsStageMatrixData* Data = (_cmsStageMatrixData*) mpe ->Data;

    return Data -> Double;
}