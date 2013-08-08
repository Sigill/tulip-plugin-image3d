#include <tulip/ImportModule.h>
#include <tulip/TulipPluginHeaders.h>
#include <tulip/StringCollection.h>
#include <math.h>
#include <stdexcept>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include "GraphFillingFunctions.h"

typedef itk::Image< int, 3 > ImageType;
typedef typename itk::ImageFileReader< ImageType > FeatureImageReaderType;

using namespace std;
using namespace tlp;

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
			throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

const string PLUGIN_NAME("Import image as integer");

namespace {
const char *paramHelp[] = {
	// 0 Image
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "File")
		HTML_HELP_BODY()
		"The image to import."
		HTML_HELP_CLOSE(),
	// 1 Connectivity
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "StringCollection")
		HTML_HELP_DEF("Values", "0;4;8")
		HTML_HELP_DEF("Default", "0")
		HTML_HELP_BODY()
		"Pixel connectivity with surrounding pixels."
		HTML_HELP_CLOSE(),
	// 2 Positionning
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Boolean")
		HTML_HELP_DEF("Default", "true")
		HTML_HELP_BODY()
		"This parameter indicates if the nodes should be positionned in space."
		HTML_HELP_CLOSE(),
	// 3 Spacing
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Double")
		HTML_HELP_DEF("Default", "1.0")
		HTML_HELP_BODY()
		"Space between adjacent pixels."
		HTML_HELP_CLOSE(),
	// 4 Property name
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "String")
		HTML_HELP_DEF("Default", "data")
		HTML_HELP_BODY()
		"Name of the integerProperty where the feature will be stored."
		HTML_HELP_CLOSE(),
};
}

class ImportImageAsInteger: public ImportModule {
public:
	PLUGININFORMATIONS(PLUGIN_NAME, "Cyrille Faucheux", "2013-08-08", "", "1.0", "File")

	ImportImageAsInteger(PluginContext *context) :
		ImportModule(context)
	{
		addDependency("Grid 3D", "1.0");

		addInParameter< std::string >          ("file::File",    paramHelp[0], "");
		addInParameter< tlp::StringCollection >("Connectivity",  paramHelp[1], "0;4;8");
		addInParameter< bool >                 ("Positionning",  paramHelp[2], "true");
		addInParameter< double >               ("Spacing",       paramHelp[3], "1.0");
		addInParameter< std::string >          ("Property name", paramHelp[4], "data");
	}
	~ImportImageAsInteger() {}

	bool importGraph()
	{
		try {
			std::string file, property_name;

			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided");

			CHECK_PROP_PROVIDED("file::File", file);
			CHECK_PROP_PROVIDED("Property name", property_name);

			if(file.empty()) {
				std::stringstream e;
				e << "\"" << file << "\" is an invalid value for the \"File\" parameter";
				throw std::runtime_error(e.str());
			}

			if(property_name.empty()) {
				std::stringstream e;
				e << "\"" << property_name << "\" is an invalid value for the \"Property name\" parameter";
				throw std::runtime_error(e.str());
			}

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

			dataSet->set< unsigned int >("Width", imageSize[0]);
			dataSet->set< unsigned int >("Height", imageSize[1]);
			dataSet->set< unsigned int >("Depth", imageSize[2]);

			if(!tlp::importGraph("Grid 3D", *dataSet, pluginProgress, graph))
				throw std::runtime_error("Unable to create the grid");

			if(pluginProgress)
				pluginProgress->setComment("Loading the image");

			tlp::IntegerProperty *featureProperty = graph->getProperty< tlp::IntegerProperty >(property_name);

			importFeature< ImageType, tlp::IntegerProperty >(image, graph, featureProperty, pluginProgress);
		} catch(std::runtime_error &ex) {
			if(pluginProgress)
				pluginProgress->setError(ex.what());
			return false;
		}

		return true;
	}
	
};

PLUGIN(ImportImageAsInteger);
