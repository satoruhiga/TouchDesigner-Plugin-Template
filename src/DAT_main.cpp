#include <string>

#include "CPlusPlus_Common.h"
#include "DAT_CPlusPlusBase.h"

////

class CustomDAT : public DAT_CPlusPlusBase
{
public:
	CustomDAT(const OP_NodeInfo *info) {}

	virtual ~CustomDAT() {}

	void setupParameters(OP_ParameterManager *manager, void *reserved1) override
	{
	}

	void getGeneralInfo(DAT_GeneralInfo *ginfo,
						const OP_Inputs *inputs,
						void *reserved1) override
	{
	}

	void execute(DAT_Output *output,
				 const OP_Inputs *inputs,
				 void *reserved) override
	{
		if (!output)
			return;

		bool isTable = false;

		if (!isTable) // is Text
		{
			output->setOutputDataType(DAT_OutDataType::Text);
			output->setText("TEST");
		}
		else
		{
			output->setOutputDataType(DAT_OutDataType::Table);
			output->setTableSize(2, 2);

			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					const std::string s = std::to_string(i) + ":" + std::to_string(j);

					output->setCellString(i, j, s.c_str());
				}
			}
		}
	}
};

////

extern "C"
{
	DLLEXPORT void FillDATPluginInfo(DAT_PluginInfo *info)
	{
		// Always return DAT_CPLUSPLUS_API_VERSION in this function.
		info->apiVersion = DATCPlusPlusAPIVersion;

		// The opType is the unique name for this CHOP. It must start with a
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

		info->customOPInfo.minInputs = 0;
		info->customOPInfo.maxInputs = 1;
	}

	DLLEXPORT DAT_CPlusPlusBase *CreateDATInstance(const OP_NodeInfo *info)
	{
		return new CustomDAT(info);
	}

	DLLEXPORT void DestroyDATInstance(DAT_CPlusPlusBase *instance)
	{
		delete (CustomDAT *)instance;
	}
};
