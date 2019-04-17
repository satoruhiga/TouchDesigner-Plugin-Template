#if OPERATOR_TYPE_TOP == 1

#include <iostream>
#include <sstream>
#include <array>
#include <memory>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>

#include "CPlusPlus_Common.h"
#include "TOP_CPlusPlusBase.h"

#include "ViveSR_API.h"
#include "ViveSR_Enums.h"

using namespace cv;

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
		//Image left, right;
		cv::Mat left{ UNDISTORTED_IMAGE_H, UNDISTORTED_IMAGE_W, CV_8UC4 };
		cv::Mat right{ UNDISTORTED_IMAGE_H, UNDISTORTED_IMAGE_W, CV_8UC4 };
	};

	aruco::Dictionary dictionary =
		aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(aruco::DICT_4X4_50));

	std::shared_ptr<Frame> frames[2];
	std::shared_ptr<Frame> *current_frame;
	std::shared_ptr<Frame> *next_frame;

	bool need_next_frame = true;
	int preroll = 30;

	Mat camMatrix, distCoeffs;
	aruco::DetectorParameters detectorParams{};
	int squareLength = 200;
	int markerLength = 150;

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

		//int margins = 0;
		//int borderBits = 1;
		//Vec4i ids{1, 2, 3, 4};
		//Mat markerImg;
		//aruco::drawCharucoDiamond(dictionary, ids, squareLength, markerLength, markerImg, margins,
		//	borderBits);

		//cv::imwrite("marker.png", markerImg);

		camMatrix = (cv::Mat_<float>(3, 3) << 301.71542358, 0, 582.72933891, 0, 303.0760498, 374.79053969, 0, 0, 1);
		distCoeffs = (cv::Mat_<float>(1, 14) << -0.7534618386, 0.146402253984, -0.000162182267957, 0.00178024848138, -0.000795359076718, -0.766160082413, 0.15605646503, -0.00266130727764, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
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

		memcpy(frame->left.data, ptr_l, frame->left.total() * frame->left.elemSize());
		memcpy(frame->right.data, ptr_r, frame->right.total() * frame->right.elemSize());

		cv::cvtColor(frame->left, frame->left, CV_RGBA2BGRA);
		cv::cvtColor(frame->right, frame->right, CV_RGBA2BGRA);

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
		{
			OP_NumericParameter p;
			p.name = "Enable";
			p.label = "Enable";
			p.page = "ViveSR";
			manager->appendToggle(p);
		}

		{
			OP_NumericParameter p;
			p.name = "Markersqsize";
			p.label = "Marker Squere Size";
			p.page = "ViveSR";
			p.defaultValues[0] = 0.05;
			manager->appendFloat(p);
		}
	}

	void getGeneralInfo(TOP_GeneralInfo* ginfo) override
	{
		ginfo->memPixelType = OP_CPUMemPixelType::RGBA8Fixed;
		ginfo->cookEveryFrame = true;
	}

	bool getOutputFormat(TOP_OutputFormat* format) override
	{
		format->width = UNDISTORTED_IMAGE_W;
		format->height = UNDISTORTED_IMAGE_H;
		format->alphaChannel = true;

		return true;
	}

	void execute(const TOP_OutputFormatSpecs* outputFormat, OP_Inputs* inputs, TOP_Context *context) override
	{
		bool enable = inputs->getParInt("Enable");
		if (enable)
		{
			initViveSR();
		}
		else
		{
			stopViveSR();
			return;
		}

		std::lock_guard<std::mutex> lock(frame_mtx_);

		int textureMemoryLocation = 0;
		uint8_t* mem = (uint8_t*)outputFormat->cpuPixelData[textureMemoryLocation];

		const auto& frame = *current_frame;
		const uint8_t* src = frame->left.data;

		for (int y = 0; y < UNDISTORTED_IMAGE_H; y++)
		{
			uint8_t* pixel = mem + UNDISTORTED_IMAGE_W * (UNDISTORTED_IMAGE_H - y - 1) * 4;

			for (int x = 0; x < outputFormat->width; x++)
			{
				pixel[0] = src[2];
				pixel[1] = src[1];
				pixel[2] = src[0];
				pixel[3] = 255;

				pixel += 4;
				src += 4;
			}
		}

		outputFormat->newCPUPixelDataLocation = textureMemoryLocation;
		need_next_frame = true;

		if (preroll--) {
			preroll = 0;
			return;
		}

		try
		{
			cv::Mat image;
			cv::cvtColor(frame->left, image, CV_BGRA2GRAY);

			std::vector< int > markerIds;
			std::vector< Vec4i > diamondIds;
			std::vector< std::vector< Point2f > > markerCorners, rejectedMarkers, diamondCorners;
			std::vector< Vec3d > rvecs, tvecs;

			aruco::detectMarkers(image, dictionary, markerCorners, markerIds, detectorParams, rejectedMarkers);

			// std::cout << "markerIds: " << markerIds.size() << std::endl;

			info_dat.clear();
			info_dat.push_back({ "id", 
				"m0",  "m1", "m2", "m3",
				"m4", "m5", "m6", "m7",
				"m8", "m9", "m10", "m11",
				"m12", "m13", "m14", "m15"});

			if (markerIds.size() > 0)
			{
				aruco::detectCharucoDiamond(image, markerCorners, markerIds,
					(float)squareLength / markerLength, diamondCorners, diamondIds,
					camMatrix, distCoeffs);

				if (diamondIds.size() > 0)
				{
					float squareSize = inputs->getParDouble("Markersqsize");
					aruco::estimatePoseSingleMarkers(diamondCorners, squareSize, camMatrix,
						distCoeffs, rvecs, tvecs);

					for (int i = 0; i < diamondIds.size(); i++)
					{
						auto id = diamondIds[0];
						std::stringstream ss;
						ss << id[0] << "-" << id[1] << "-" << id[2] << "-" << id[3];

						std::vector<std::string> info;
						info.push_back(ss.str());

						auto tvec = cv::Mat(tvecs[i]);
						auto rvec = cv::Mat(rvecs[i]);

						cv::Mat R;
						cv::Rodrigues(rvec, R);

						//R = R.t();
						//tvec = -R * tvec;

						cv::Mat T(4, 4, R.type());
						T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1;
						T(cv::Range(0, 3), cv::Range(3, 4)) = tvec * 1;
						double *p = T.ptr<double>(3);
						p[0] = p[1] = p[2] = 0; p[3] = 1;

						// std::cout << T << std::endl;

						p = T.ptr<double>(0);

						for (int n = 0; n < 16; n++) {
							info.push_back(std::to_string(p[n]));
						}

						info_dat.emplace_back(info);
					}
				}
			}
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	std::vector<std::vector<std::string>> info_dat;

	virtual bool getInfoDATSize(OP_InfoDATSize* infoSize)
	{
		int rows = 0;
		int cols = 0;

		if (info_dat.size())
		{
			rows = info_dat.size();
			cols = info_dat[0].size();
		}

		infoSize->rows = rows;
		infoSize->cols = cols;
		infoSize->byColumn = false;

		return true;
	}

	virtual void getInfoDATEntries(int32_t index,
		int32_t nEntries,
		OP_InfoDATEntries* entries)
	{
		auto& row = info_dat[index];

		for (int i = 0; i < nEntries; i++)
		{
			entries->values[i] = (char*)row[i].c_str();
		}
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
