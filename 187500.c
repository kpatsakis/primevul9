int IsInSet(int Type, _cmsParametricCurvesCollection* c)
{
    int i;

    for (i=0; i < c ->nFunctions; i++)
        if (abs(Type) == c ->FunctionTypes[i]) return i;

    return -1;
}