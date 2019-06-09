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

	void setupParameters(OP_ParameterManager* manager, void* reserved1) override
	{}

	void getGeneralInfo(CHOP_GeneralInfo* info, const OP_Inputs *inputs, void* reserved1) override
	{}

	bool getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs *inputs, void *reserved1) override
	{
		info->numSamples = 600;
		info->numChannels = 1;

		return true;
	}

	void getChannelName(int32_t index, OP_String *name,
		const OP_Inputs *inputs, void* reserved1) override
	{
		name->setString("chan1");
	}

	void execute(CHOP_Output* output, const OP_Inputs* inputs, void* reserved1) override
	{
		for (int i = 0; i < output->numSamples; i++)
		{
			float d = (float)i / (output->numSamples - 1);
			output->channels[0][i] = sin(d * 3.1415 * 2);
		}
	}
};

////

extern "C"
{
	DLLEXPORT void FillCHOPPluginInfo(CHOP_PluginInfo *info)
	{
		// Always set this to CHOPCPlusPlusAPIVersion.
		info->apiVersion = CHOPCPlusPlusAPIVersion;

		// The opType is the unique name for this CHOP. It must start with a 
		// capital A-Z character, and all the following characters must lower case
		// or numbers (a-z, 0-9)
		info->customOPInfo.opType->setString("Testop");

		// The opLabel is the text that will show up in the OP Create Dialog
		info->customOPInfo.opLabel->setString("Test OP");

		// Information about the author of this OP
		info->customOPInfo.authorName->setString("Author Name");
		info->customOPInfo.authorEmail->setString("email@email.com");

		// This CHOP can work with 0 inputs
		info->customOPInfo.minInputs = 0;

		// It can accept up to 1 input though, which changes it's behavior
		info->customOPInfo.maxInputs = 1;
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
