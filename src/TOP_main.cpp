#if OPERATOR_TYPE_TOP == 1

#include "CPlusPlus_Common.h"
#include "TOP_CPlusPlusBase.h"

////

class PROJECT_NAME : public TOP_CPlusPlusBase
{
public:

	PROJECT_NAME(const OP_NodeInfo* info, TOP_Context *context)
	{}

	virtual ~PROJECT_NAME()
	{}

	void getGeneralInfo(TOP_GeneralInfo* ginfo) override
	{
		ginfo->memPixelType = OP_CPUMemPixelType::RGBA8Fixed;
	}

	bool getOutputFormat(TOP_OutputFormat* format)
	{
		return false;
	}

	void execute(const TOP_OutputFormatSpecs* outputFormat, OP_Inputs* inputs, TOP_Context *context) override
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