#include "CPlusPlus_Common.h"
#include "TOP_CPlusPlusBase.h"

using namespace TD;

////


class CustomTOP : public TOP_CPlusPlusBase
{
public:
	TOP_Context* myContext;
	
	CustomTOP(const OP_NodeInfo *info, TOP_Context* context)
		: myContext(context)
	{}

	virtual ~CustomTOP()
	{}

	void setupParameters(OP_ParameterManager* manager, void* reserved1) override
	{}

	void getGeneralInfo(TOP_GeneralInfo* ginfo, const OP_Inputs*, void *reserved1) override
	{
		ginfo->cookEveryFrameIfAsked = true;
	}

	void execute(TOP_Output* output, const OP_Inputs* inputs, void* reserved1) override
	{
		TOP_UploadInfo info;
		info.textureDesc.texDim = OP_TexDim::e2D;
		info.textureDesc.width = 256;
		info.textureDesc.height = 256;
		info.textureDesc.pixelFormat = OP_PixelFormat::BGRA8Fixed;
		info.colorBufferIndex = 0;

		size_t byteSize = info.textureDesc.width * info.textureDesc.height * 4 * sizeof(uint8_t);
		OP_SmartRef<TOP_Buffer> buf = myContext->createOutputBuffer(byteSize, TOP_BufferFlags::None, nullptr);

		uint8_t* mem = (uint8_t*)buf->data;

		for (int y = 0; y < info.textureDesc.height; y++)
		{
			uint8_t* pixel = mem + info.textureDesc.width * y * 4;
			float v = (float)y / (info.textureDesc.height - 1);

			for (int x = 0; x < info.textureDesc.width; x++)
			{
				float u = (float)x / (info.textureDesc.width - 1);

				pixel[0] = u * 255;
				pixel[1] = v * 255;
				pixel[2] = 0;
				pixel[3] = 255;

				pixel += 4;
			}
		}

		output->uploadBuffer(&buf, info, nullptr);
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
		info->executeMode = TOP_ExecuteMode::CPUMem;

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

	DLLEXPORT TOP_CPlusPlusBase* CreateTOPInstance(const OP_NodeInfo* info, TOP_Context* context)
	{
		return new CustomTOP(info, context);
	}

	DLLEXPORT void DestroyTOPInstance(TOP_CPlusPlusBase* instance, TOP_Context *context)
	{
		delete (CustomTOP*)instance;
	}
};
