#include <tulip/TulipPluginHeaders.h>
#include <tulip/IntegerProperty.h>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

#include "PluginUtils.h"
#include "GraphFillingFunctions.h"

#include <sstream>
#include <stdexcept>

typedef itk::VectorImage< int, 3 > ImageType;
typedef typename itk::ImageFileReader< ImageType > FeatureImageReaderType;

namespace {
	const char
		* paramHelp[] = {
			//File name
			HTML_HELP_OPEN()
				HTML_HELP_DEF("Type", "File")
				HTML_HELP_BODY()
				"The image to import."
				HTML_HELP_CLOSE(),
			// Integer property
			HTML_HELP_OPEN()
				HTML_HELP_DEF("Type", "IntegerVectorProperty")
				HTML_HELP_DEF("Default", "data")
				HTML_HELP_BODY()
				"The IntegerVectorProperty where the data will be stored."
				HTML_HELP_CLOSE(),
		};
}

class LoadIntegerVectorPropertyFromImage: public tlp::Algorithm {
	std::string file;
	tlp::IntegerVectorProperty *property;

public:
	PLUGININFORMATIONS("Load integervector property from image", "Cyrille FAUCHEUX", "2013-08-01", "", "1.0", "Image")

	LoadIntegerVectorPropertyFromImage(const tlp::PluginContext *context):
		tlp::Algorithm(context)
	{
		addInParameter< std::string >               ("file::Image", paramHelp[0], "");
		addInParameter< tlp::IntegerVectorProperty >("Property",    paramHelp[1], "data");
	}

	~LoadIntegerVectorPropertyFromImage() {}

	bool check(std::string &err) {
		try {
			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided.");

			CHECK_PROP_PROVIDED("file::Image", this->file);
			CHECK_PROP_PROVIDED("Property", this->property);

			if(file.empty()) {
				std::stringstream e;
				e << "\"" << file << "\" is an invalid value for the \"File\" parameter";
				throw std::runtime_error(e.str());
			}
		} catch (std::runtime_error &ex) {
			err.assign(ex.what());
			return false;
		}

		return true;
	}

	bool run() {
		try {
			typename FeatureImageReaderType::Pointer featureImageReader = FeatureImageReaderType::New();
			featureImageReader->SetFileName(file);
			try {
				featureImageReader->Update();
			} catch ( itk::ExceptionObject & err ) {
				std::stringstream e;
				e << "The image located at \"" << file << "\" is not readable";
				throw std::runtime_error(e.str());
			}

			typename ImageType::Pointer image = featureImageReader->GetOutput();
			typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

			if(pluginProgress)
				pluginProgress->setComment("Loading the image");

			importVectorFeature< ImageType, tlp::IntegerVectorProperty >(image, graph, property, pluginProgress);
		} catch(std::runtime_error &ex) {
			if(pluginProgress)
				pluginProgress->setError(ex.what());
			return false;
		}

		return true;
	}
};

PLUGIN(LoadIntegerVectorPropertyFromImage)
