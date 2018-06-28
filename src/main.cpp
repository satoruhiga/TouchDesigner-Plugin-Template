#include "CPlusPlus_Common.h"
#include "CHOP_CPlusPlusBase.h"
#include "TOP_CPlusPlusBase.h"

#pragma region OP_Defines

#if OPERATOR_TYPE_CHOP == 1
#define CPLUSPLUS_BASE_CLASS CHOP_CPlusPlusBase
#endif

#if OPERATOR_TYPE_TOP == 1
#define CPLUSPLUS_BASE_CLASS TOP_CPlusPlusBase
#endif

#if OPERATOR_TYPE_SOP == 1
#define CPLUSPLUS_BASE_CLASS SOP_CPlusPlusBase
#endif

#pragma endregion OP_Defines

////

class PROJECT_NAME : public CPLUSPLUS_BASE_CLASS
{
public:

	PROJECT_NAME(const OP_NodeInfo* info)
	{}

	//PROJECT_NAME(const OP_NodeInfo* info, TOP_Context *context)
	//{}

	virtual ~PROJECT_NAME()
	{}

	void execute(const CHOP_Output* output, OP_Inputs* inputs, void*) override
	{}

	//void execute(const TOP_OutputFormatSpecs* outputFormat, OP_Inputs* inputs, TOP_Context *context) override
	//{}

	//void execute(SOP_Output* output, OP_Inputs* inputs, void* reserved) override
	//{}

};

////

#pragma region OP_DLLEXPORTs

#if OPERATOR_TYPE_CHOP == 1

extern "C"
{
	DLLEXPORT int32_t GetCHOPAPIVersion(void)
	{
		return CHOP_CPLUSPLUS_API_VERSION;
	}

	DLLEXPORT CHOP_CPlusPlusBase* CreateCHOPInstance(const OP_NodeInfo* info)
	{
		return new PROJECT_NAME(info);
	}

	DLLEXPORT void DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
	{
		delete (PROJECT_NAME*)instance;
	}
};

#endif

#if OPERATOR_TYPE_TOP == 1

extern "C"
{
	DLLEXPORT TOP_PluginInfo GetTOPPluginInfo(void)
	{
		TOP_PluginInfo info;
		info.apiVersion = TOPCPlusPlusAPIVersion;
		info.executeMode = TOP_ExecuteMode::CPUMemWriteOnly;
		//info.executeMode = TOP_ExecuteMode::OpenGL_FBO;
		return info;
	}

	DLLEXPORT TOP_CPlusPlusBase* CreateTOPInstance(const OP_NodeInfo* info, TOP_Context *context)
	{
		return new PROJECT_NAME(info, context);
	}

	DLLEXPORT void DestroyTOPInstance(TOP_CPlusPlusBase* instance, TOP_Context *context)
	{
		//context->beginGLCommands();
		delete (PROJECT_NAME*)instance;
		//context->endGLCommands();
	}
};

#endif

#if OPERATOR_TYPE_SOP == 1

extern "C"
{

	DLLEXPORT int32_t GetSOPAPIVersion(void)
	{
		return SOP_CPLUSPLUS_API_VERSION;
	}

	DLLEXPORT SOP_CPlusPlusBase* CreateSOPInstance(const OP_NodeInfo* info)
	{
		return new PROJECT_NAME(info);
	}

	DLLEXPORT void DestroySOPInstance(SOP_CPlusPlusBase* instance)
	{
		delete (PROJECT_NAME*)instance;
	}
};

#endif

#pragma endregion OP_DLLEXPORTs