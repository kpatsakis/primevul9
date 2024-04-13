cmsBool _cmsRegisterParametricCurvesPlugin(cmsContext id, cmsPluginBase* Data)
{
    cmsPluginParametricCurves* Plugin = (cmsPluginParametricCurves*) Data;
    _cmsParametricCurvesCollection* fl;

    if (Data == NULL) {

          ParametricCurves =  &DefaultCurves;
          return TRUE;
    }

    fl = (_cmsParametricCurvesCollection*) _cmsPluginMalloc(id, sizeof(_cmsParametricCurvesCollection));
    if (fl == NULL) return FALSE;

    // Copy the parameters
    fl ->Evaluator  = Plugin ->Evaluator;
    fl ->nFunctions = Plugin ->nFunctions;

    // Make sure no mem overwrites
    if (fl ->nFunctions > MAX_TYPES_IN_LCMS_PLUGIN)
        fl ->nFunctions = MAX_TYPES_IN_LCMS_PLUGIN;

    // Copy the data
    memmove(fl->FunctionTypes,  Plugin ->FunctionTypes,   fl->nFunctions * sizeof(cmsUInt32Number));
    memmove(fl->ParameterCount, Plugin ->ParameterCount,  fl->nFunctions * sizeof(cmsUInt32Number));

    // Keep linked list
    fl ->Next = ParametricCurves;
    ParametricCurves = fl;

    // All is ok
    return TRUE;
}