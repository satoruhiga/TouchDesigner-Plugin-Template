#if OPERATOR_TYPE_TOP == 1

#include "CPlusPlus_Common.h"
#include "TOP_CPlusPlusBase.h"

////

class OPERATOR_NAME : public TOP_CPlusPlusBase
{
public:

	OPERATOR_NAME(const OP_NodeInfo* info, TOP_Context *context)
	{}

	virtual ~OPERATOR_NAME()
	{}

	void setupParameters(OP_ParameterManager* manager, void* reserved1) override
	{}

	void getGeneralInfo(TOP_GeneralInfo* ginfo, const OP_Inputs*, void *reserved1) override
	{
		ginfo->memPixelType = OP_CPUMemPixelType::RGBA8Fixed;
	}

	bool getOutputFormat(TOP_OutputFormat* format, const OP_Inputs*, void* reserved1) override
	{
		return false;
	}

	void execute(TOP_OutputFormatSpecs* outputFormat, const OP_Inputs* inputs, TOP_Context *context, void* reserved1) override
	{
		int textureMemoryLocation = 0;

		uint8_t* mem = (uint8_t*)outputFormat->cpuPixelData[textureMemoryLocation];

		for (int y = 0; y < outputFormat->height; y++)
		{
			uint8_t* pixel = mem + outputFormat->width * y * 4;
			float v = (float)y / (outputFormat->height - 1);

			for (int x = 0; x < outputFormat->width; x++)
			{
				float u = (float)x / (outputFormat->width - 1);

				pixel[0] = u * 255;
				pixel[1] = v * 255;
				pixel[2] = 0;
				pixel[3] = 255;

				pixel += 4;
			}
		}

		outputFormat->newCPUPixelDataLocation = textureMemoryLocation;
	}

};

////

extern "C"
{
	DLLEXPORT void FillTOPPluginInfo(TOP_PluginInfo *info)
	{
		// This must always be set to this constant
		info->apiVersion = TOPCPlusPlusAPIVersion;

		// Change this to change the executeMode behavior of this plugin.
		info->executeMode = TOP_ExecuteMode::CPUMemWriteOnly;

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

		// This TOP works with 0 or 1 inputs connected
		info->customOPInfo.minInputs = 0;
		info->customOPInfo.maxInputs = 1;
	}

	DLLEXPORT TOP_CPlusPlusBase* CreateTOPInstance(const OP_NodeInfo* info, TOP_Context *context)
	{
		return new OPERATOR_NAME(info, context);
	}

	DLLEXPORT void DestroyTOPInstance(TOP_CPlusPlusBase* instance, TOP_Context *context)
	{
		//context->beginGLCommands();
		delete (OPERATOR_NAME*)instance;
		//context->endGLCommands();
	}
};

#endif
