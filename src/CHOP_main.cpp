#if OPERATOR_TYPE_CHOP == 1

#include "CPlusPlus_Common.h"
#include "CHOP_CPlusPlusBase.h"

////

class PROJECT_NAME : public CHOP_CPlusPlusBase
{
public:

	PROJECT_NAME(const OP_NodeInfo* info)
	{}

	virtual ~PROJECT_NAME()
	{}

	void execute(const CHOP_Output* output, OP_Inputs* inputs, void*) override
	{}

};

////

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