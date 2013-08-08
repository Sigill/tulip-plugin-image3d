#include <tulip/ImportModule.h>
#include <tulip/TulipPluginHeaders.h>
#include <tulip/StringCollection.h>
#include <math.h>
#include <stdexcept>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

#include "GraphFillingFunctions.h"

typedef itk::VectorImage< double, 3 > ImageType;
typedef typename itk::ImageFileReader< ImageType > FeatureImageReaderType;

using namespace std;
using namespace tlp;

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
			throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

const string PLUGIN_NAME("Import image as color");

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
	// 4 Convert to grayscale
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Boolean")
		HTML_HELP_DEF("Default", "false")
		HTML_HELP_BODY()
		"Indicates if the color should be converted to grayscale."
		HTML_HELP_CLOSE(),
};
}

class ImportImageAsColor: public ImportModule {
public:
	PLUGININFORMATIONS(PLUGIN_NAME, "Cyrille Faucheux", "2013-08-08", "", "1.0", "File")

	ImportImageAsColor(PluginContext *context) :
		ImportModule(context)
	{
		addDependency("Grid 3D", "1.0");

		addInParameter< std::string >          ("file::File",            paramHelp[0], "");
		addInParameter< tlp::StringCollection >("Connectivity",          paramHelp[1], "0;4;8");
		addInParameter< bool >                 ("Positionning",          paramHelp[2], "true");
		addInParameter< double >               ("Spacing",               paramHelp[3], "1.0");
		addInParameter< bool >                 ("Convert to grayscale",  paramHelp[4], "false");
	}
	~ImportImageAsColor() {}

	bool importGraph()
	{
		try {
			std::string file, property_name;
			bool convert_to_grayscale;

			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided");

			CHECK_PROP_PROVIDED("file::File", file);
			CHECK_PROP_PROVIDED("Convert to grayscale", convert_to_grayscale);

			if(file.empty()) {
				std::stringstream e;
				e << "\"" << file << "\" is an invalid value for the \"File\" parameter";
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

			tlp::ColorProperty *viewColor = graph->getProperty< tlp::ColorProperty >("viewColor");

			importColor< ImageType >(image, graph, viewColor, convert_to_grayscale, pluginProgress);
		} catch(std::runtime_error &ex) {
			if(pluginProgress)
				pluginProgress->setError(ex.what());
			return false;
		}

		return true;
	}
	
};

PLUGIN(ImportImageAsColor);
