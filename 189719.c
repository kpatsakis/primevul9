msc_engine *modsecurity_create(apr_pool_t *mp, int processing_mode) {
    msc_engine *msce = NULL;

    msce = apr_pcalloc(mp, sizeof(msc_engine));
    if (msce == NULL) return NULL;

    msce->mp = mp;
    msce->processing_mode = processing_mode;

    msce->msre = msre_engine_create(msce->mp);
    if (msce->msre == NULL) return NULL;
    msre_engine_register_default_variables(msce->msre);
    msre_engine_register_default_operators(msce->msre);
    msre_engine_register_default_tfns(msce->msre);
    msre_engine_register_default_actions(msce->msre);
    // TODO: msre_engine_register_default_reqbody_processors(msce->msre);

    return msce;
}