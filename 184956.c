void CClient::DemoRecorder_HandleAutoStart()
{
	if(g_Config.m_ClAutoDemoRecord)
	{
		DemoRecorder_Stop();
		DemoRecorder_Start("auto/autorecord", true);
		if(g_Config.m_ClAutoDemoMax)
		{
			// clean up auto recorded demos
			CFileCollection AutoDemos;
			AutoDemos.Init(Storage(), "demos/auto", "autorecord", ".demo", g_Config.m_ClAutoDemoMax);
		}
	}
}