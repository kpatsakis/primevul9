PlayerBase::PlayerTypes PlayerGeneric::getPreferredPlayerType(XModule* module)
{
	if (module == NULL)
		return PlayerBase::PlayerType_INVALID;
		
	switch (module->getType())
	{
		case XModule::ModuleType_669:
		case XModule::ModuleType_FAR:
#ifndef MILKYTRACKER
			return PlayerBase::PlayerType_FAR;
			break;
#endif
		case XModule::ModuleType_IT:
#ifndef MILKYTRACKER
			return PlayerBase::PlayerType_IT;
			break;
#endif
		case XModule::ModuleType_UNKNOWN: // just assume our standard player can handle this
		//case XModule::ModuleType_669:
		case XModule::ModuleType_AMF:
		case XModule::ModuleType_AMS:
		case XModule::ModuleType_CBA:
		case XModule::ModuleType_DBM:
		case XModule::ModuleType_DIGI:
		case XModule::ModuleType_DSM:
		case XModule::ModuleType_DSm:
		case XModule::ModuleType_DTM_1:
		case XModule::ModuleType_DTM_2:
		case XModule::ModuleType_GDM:
		case XModule::ModuleType_GMC:
		case XModule::ModuleType_IMF:
		case XModule::ModuleType_MDL:
		case XModule::ModuleType_MOD:
		case XModule::ModuleType_MTM:
		case XModule::ModuleType_MXM:
		case XModule::ModuleType_OKT:
		case XModule::ModuleType_PLM:
		case XModule::ModuleType_PSM:
		case XModule::ModuleType_PTM:
		case XModule::ModuleType_S3M:
		case XModule::ModuleType_STM:
		case XModule::ModuleType_SFX:
		case XModule::ModuleType_UNI:
		case XModule::ModuleType_ULT:
		case XModule::ModuleType_XM:
		case XModule::ModuleType_NONE:
			return PlayerBase::PlayerType_Generic;
			break;
			
		default:
			return PlayerBase::PlayerType_INVALID;
	}
}