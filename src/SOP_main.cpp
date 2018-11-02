#if OPERATOR_TYPE_SOP == 1

#include "CPlusPlus_Common.h"
#include "SOP_CPlusPlusBase.h"

////

class PROJECT_NAME : public SOP_CPlusPlusBase
{
public:

	PROJECT_NAME(const OP_NodeInfo* info)
	{}

	virtual ~PROJECT_NAME()
	{}

	void setupParameters(OP_ParameterManager* manager) override
	{}

	void execute(SOP_Output* output, OP_Inputs* inputs, void* reserved) override
	{
		int N = 32;

		for (int i = 0; i < N; i++)
		{
			float d = (float)i / (N - 1);

			float x = sin(d * 3.1415 * 2);
			float y = cos(d * 3.1415 * 2);

			output->addPoint(x, y, 0);
		}
	}

	void executeVBO(SOP_VBOOutput* output, OP_Inputs*, void* reserved) override
	{}

};

////

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