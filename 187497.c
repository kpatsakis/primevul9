_cmsParametricCurvesCollection *GetParametricCurveByType(int Type, int* index)
{
    _cmsParametricCurvesCollection* c;
    int Position;

    for (c = ParametricCurves; c != NULL; c = c ->Next) {

        Position = IsInSet(Type, c);

        if (Position != -1) {
            if (index != NULL)
                *index = Position;
            return c;
        }
    }

    return NULL;
}