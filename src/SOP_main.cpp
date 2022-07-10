#include "CPlusPlus_Common.h"
#include "SOP_CPlusPlusBase.h"

using namespace TD;

////

class CustomSOP : public SOP_CPlusPlusBase
{
public:

	CustomSOP(const OP_NodeInfo* info)
	{}

	virtual ~CustomSOP()
	{}

	void setupParameters(OP_ParameterManager* manager, void* reserved1) override
	{}

	void execute(SOP_Output* output, const OP_Inputs* inputs, void* reserved) override
	{
		int N = 32;

		for (int i = 0; i < N; i++)
		{
			float d = (float)i / (N - 1);

			float x = sin(d * 3.1415 * 2);
			float y = cos(d * 3.1415 * 2);
			
			output->addPoint(Position(x, y, 0));
		}
	}

	void executeVBO(SOP_VBOOutput* output, const OP_Inputs*, void* reserved) override
	{}

};

////

extern "C"
{

	DLLEXPORT void FillSOPPluginInfo(SOP_PluginInfo *info)
	{
		// Always return SOP_CPLUSPLUS_API_VERSION in this function.
		info->apiVersion = SOPCPlusPlusAPIVersion;

		// The opType is the unique name for this TOP. It must start with a 
		// capital A-Z character, and all the following characters must lower case
		// or numbers (a-z, 0-9)
		info->customOPInfo.opType->setString("Testop");

		// The opLabel is the text that will show up in the OP Create Dialog
		info->customOPInfo.opLabel->setString("Test OP");

		// Will be turned into a 3 letter icon on the nodes
		info->customOPInfo.opIcon->setString("TST");

		// Information about the author of this OP
		info->customOPInfo.authorName->setString("Author Name");
		info->customOPInfo.authorEmail->setString("email@email.com");

		// This SOP works with 0 or 1 inputs
		info->customOPInfo.minInputs = 0;
		info->customOPInfo.maxInputs = 1;

	}
	DLLEXPORT SOP_CPlusPlusBase* CreateSOPInstance(const OP_NodeInfo* info)
	{
		return new CustomSOP(info);
	}

	DLLEXPORT void DestroySOPInstance(SOP_CPlusPlusBase* instance)
	{
		delete (CustomSOP*)instance;
	}
};
