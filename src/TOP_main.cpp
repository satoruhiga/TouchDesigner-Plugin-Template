#if OPERATOR_TYPE_TOP == 1

#include <iostream>
#include <array>
#include <memory>
#include <mutex>

#include "CPlusPlus_Common.h"
#include "TOP_CPlusPlusBase.h"

#include "ViveSR_API.h"
#include "ViveSR_Enums.h"

#define UNDISTORTED_IMAGE_W 1150
#define UNDISTORTED_IMAGE_H 750
#define UNDISTORTED_IMAGE_C 4

#define CHECK_ERROR(CODE) ([&](){ bool res = CODE; if (!res) { std::cerr << "ERROR: " << #CODE << std::endl; } return res; })();
#define ASSERT_ERROR(CODE) ([&](){ bool res = CODE; if (!res) { throw std::runtime_error(std::string("ERROR: ") + #CODE); } return res; })();

////

class PROJECT_NAME;
static PROJECT_NAME* g_self_ptr = nullptr;

class PROJECT_NAME : public TOP_CPlusPlusBase
{
public:

	bool inited_ = false;
	int ID_SEETHROUGH;
	std::mutex frame_mtx_;

	using Image = std::array<uint8_t, 3450000>;

	struct Frame {
		Image left, right;
	};

	std::shared_ptr<Frame> frames[2];
	std::shared_ptr<Frame> *current_frame;
	std::shared_ptr<Frame> *next_frame;

	bool need_next_frame = true;

	bool initViveSR()
	{
		if (inited_) return true;
		inited_ = true;

		int res;

		frames[0] = std::make_shared<Frame>();
		frames[1] = std::make_shared<Frame>();

		current_frame = &frames[0];
		next_frame = &frames[1];

		ASSERT_ERROR(ViveSR_Initial() == ViveSR::SR_Error::WORK);

		ViveSR_EnableLog(true);
		ViveSR_SetLogLevel(ViveSR::SR_LogLevel::SRLOG_LEVEL_MAX);

		res = CHECK_ERROR(ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_SEETHROUGH, &ID_SEETHROUGH) == ViveSR::SR_Error::WORK);
		
		//ViveSR_GetCameraParams(&CameraParameters);

		CHECK_ERROR(ViveSR_SetParameterBool(ID_SEETHROUGH, ViveSR::SeeThrough::Param::VR_INIT, true) == ViveSR::SR_Error::WORK);
		CHECK_ERROR(ViveSR_SetParameterInt(ID_SEETHROUGH, ViveSR::SeeThrough::Param::VR_INIT_TYPE, ViveSR::SeeThrough::InitType::BACKGROUND) == ViveSR::SR_Error::WORK);


		CHECK_ERROR(ViveSR_StartModule(ID_SEETHROUGH) == ViveSR::SR_Error::WORK);
		
		ViveSR_RegisterCallback(ID_SEETHROUGH, ViveSR::SeeThrough::Callback::BASIC, UndistortedCallback_);
	}

	static void UndistortedCallback_(int key) {
		g_self_ptr->UndistortedCallback(key);
	}

	void UndistortedCallback(int key)
	{
		if (!need_next_frame) return;

		char *ptr_l, *ptr_r;

		ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_LEFT, (void**)&ptr_l);
		ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_RIGHT, (void**)&ptr_r);
		
		auto frame = *next_frame;

		memcpy(frame->left.data(), ptr_l, frame->left.size());
		memcpy(frame->right.data(), ptr_r, frame->right.size());

		std::lock_guard<std::mutex> lock(frame_mtx_);
		std::swap(next_frame, current_frame);
		need_next_frame = false;
	}

	///

	bool stopViveSR()
	{
		if (!inited_) return true;
		inited_ = false;
		
		return CHECK_ERROR(ViveSR_Stop() == ViveSR::SR_Error::WORK);
	}

	PROJECT_NAME(const OP_NodeInfo* info, TOP_Context *context)
	{
		g_self_ptr = this;
		initViveSR();
	}

	virtual ~PROJECT_NAME()
	{
		stopViveSR();
	}

	void setupParameters(OP_ParameterManager* manager) override
	{
		OP_NumericParameter p;
		p.name = "Enable";
		p.label = "Enable";
		p.page = "ViveSR";
		manager->appendToggle(p);
	}

	void getGeneralInfo(TOP_GeneralInfo* ginfo) override
	{
		ginfo->memPixelType = OP_CPUMemPixelType::RGBA8Fixed;
		ginfo->cookEveryFrame = true;
	}

	bool getOutputFormat(TOP_OutputFormat* format) override
	{
		initViveSR();

		format->width = UNDISTORTED_IMAGE_W;
		format->height = UNDISTORTED_IMAGE_H;
		format->alphaChannel = true;

		return true;
	}

	void execute(const TOP_OutputFormatSpecs* outputFormat, OP_Inputs* inputs, TOP_Context *context) override
	{
		std::lock_guard<std::mutex> lock(frame_mtx_);

		int textureMemoryLocation = 0;
		uint8_t* mem = (uint8_t*)outputFormat->cpuPixelData[textureMemoryLocation];

		const auto& frame = *current_frame;
		const uint8_t* src = frame->left.data();

		for (int y = 0; y < UNDISTORTED_IMAGE_H; y++)
		{
			uint8_t* pixel = mem + UNDISTORTED_IMAGE_W * (UNDISTORTED_IMAGE_H - y - 1) * 4;

			for (int x = 0; x < outputFormat->width; x++)
			{
				pixel[0] = src[0];
				pixel[1] = src[1];
				pixel[2] = src[2];
				pixel[3] = 255;

				pixel += 4;
				src += 4;
			}
		}

		outputFormat->newCPUPixelDataLocation = textureMemoryLocation;

		need_next_frame = true;
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
