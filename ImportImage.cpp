#include <tulip/ImportModule.h>
#include <tulip/TulipPluginHeaders.h>
#include <tulip/StringCollection.h>
#include <math.h>
#include <stdexcept>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

#include "GraphFillingFunctions2.h"

typedef itk::VectorImage< double, 3 > ImageType;
typedef typename itk::ImageFileReader< ImageType > ImageReaderType;

using namespace std;
using namespace tlp;

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
			throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

const string PLUGIN_NAME("Import image");

namespace {
const char *paramHelp[] = {
	// 0 Image
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "File")
		HTML_HELP_BODY()
		"The image to import."
		HTML_HELP_CLOSE(),

	// 1 Neighborhood radius
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Double")
		HTML_HELP_DEF("Default", "0")
		HTML_HELP_BODY()
		"The radius of the generated neighborhood (this is not affected by the Spacing parameter)."
		HTML_HELP_CLOSE(),

	// 2 Neighborhood type
	HTML_HELP_OPEN()
		HTML_HELP_DEF("type", "String")
		HTML_HELP_DEF("Values", "Circular;Square")
		HTML_HELP_DEF("Default", "Circular")
		HTML_HELP_BODY()
		"The type of neighborhood to build."
		HTML_HELP_CLOSE(),

	// 3 Positionning
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Boolean")
		HTML_HELP_DEF("Default", "true")
		HTML_HELP_BODY()
		"This parameter indicates if the nodes should be positionned in space."
		HTML_HELP_CLOSE(),

	// 4 Spacing
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Double")
		HTML_HELP_DEF("Default", "1.0")
		HTML_HELP_BODY()
		"Space between adjacent pixels."
		HTML_HELP_CLOSE(),

	// 5 Convert to grayscale
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Boolean")
		HTML_HELP_DEF("Default", "false")
		HTML_HELP_BODY()
		"Indicates if the color should be converted to grayscale."
		HTML_HELP_CLOSE(),

	// 6 Property type
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "StringCollection")
		HTML_HELP_BODY()
		"The type of the of the property: Color, Integer, IntegerVector, Double, DoubleVector, Boolean."
		HTML_HELP_CLOSE(),

	// 7 Property name
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "String")
		HTML_HELP_DEF("Default", "data")
		HTML_HELP_BODY()
		"The name of the property."
		HTML_HELP_CLOSE(),
};
}

class ImportImage: public ImportModule {
private:
	enum property_t { COLOR, INTEGER, INTEGERVECTOR, DOUBLE, DOUBLEVECTOR, BOOLEAN };
	property_t property_type;
	std::string property_name;

public:
	PLUGININFORMATIONS(PLUGIN_NAME, "Cyrille Faucheux", "2013-08-18", "", "1.0", "File")

	ImportImage(PluginContext *context) :
		ImportModule(context)
	{
		addDependency("Grid 3D", "1.1");

		addInParameter< std::string >          ("file::File",           paramHelp[0], "");
		addInParameter< tlp::StringCollection >("Neighborhood type",    paramHelp[1], "Circular;Square");
		addInParameter< double >               ("Neighborhood radius",  paramHelp[2], "0");
		addInParameter< bool >                 ("Positionning",         paramHelp[3], "true");
		addInParameter< double >               ("Spacing",              paramHelp[4], "1.0");
		addInParameter< tlp::StringCollection >("Property type",        paramHelp[6], "Color;Integer;IntegerVector;Double;DoubleVector;Boolean");
		addInParameter< std::string >          ("Property name",        paramHelp[7], "data");
		addInParameter< bool >                 ("Convert to grayscale", paramHelp[5], "false");
	}
	~ImportImage() {}

	bool importGraph()
	{
		try {
			std::string file;
			bool convert_to_grayscale;
			tlp::StringCollection property_type_tmp;

			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided");

			CHECK_PROP_PROVIDED("file::File", file);
			CHECK_PROP_PROVIDED("Convert to grayscale", convert_to_grayscale);
			CHECK_PROP_PROVIDED("Property type", property_type_tmp);
			CHECK_PROP_PROVIDED("Property name", property_name);

			if(file.empty()) {
				std::stringstream e; e << "The \"File\" parameter cannot be empty";
				throw std::runtime_error(e.str());
			}

			if(this->property_name.empty()) {
				std::stringstream e; e << "The \"Property name\" parameter cannot be empty";
				throw std::runtime_error(e.str());
			}

			if(property_type_tmp.getCurrentString().compare("Color") == 0) {
				this->property_type = COLOR;
			} else if(property_type_tmp.getCurrentString().compare("Integer") == 0) {
				this->property_type = INTEGER;
			} else if(property_type_tmp.getCurrentString().compare("IntegerVector") == 0) {
				this->property_type = INTEGERVECTOR;
			}  else if(property_type_tmp.getCurrentString().compare("Double") == 0) {
				this->property_type = DOUBLE;
			} else if(property_type_tmp.getCurrentString().compare("DoubleVector") == 0) {
				this->property_type = DOUBLEVECTOR;
			} else if(property_type_tmp.getCurrentString().compare("Boolean") == 0) {
				this->property_type = BOOLEAN;
			} else {
				throw std::runtime_error("Unknown property type.");
			}

			typename ImageReaderType::Pointer imageReader = ImageReaderType::New();
			imageReader->SetFileName(file);
			try {
				imageReader->Update();
			} catch ( itk::ExceptionObject & err ) {
				std::stringstream e;
				e << "The image located at \"" << file << "\" is not readable";
				throw std::runtime_error(e.str());
			}

			typename ImageType::Pointer image = imageReader->GetOutput();
			typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
			const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();

			switch(this->property_type) {
				case COLOR:
					if((numberOfComponents != 1) && (numberOfComponents != 3)) {
						throw std::runtime_error("To import the image as a ColorProperty, it must have either 1 or 3 components per pixel.");
					}
					break;
				case INTEGER:
				case DOUBLE:
				case BOOLEAN:
					/* ITK returns 3 when reading a grayscale BMP file
					if(1 != numberOfComponents)
						throw std::runtime_error("To import the image as a IntegerProperty, a DoubleProperty or a BooleanProperty, it must have 1 components per pixel.");
					*/
					break;
			}

			dataSet->set< unsigned int >("Width", imageSize[0]);
			dataSet->set< unsigned int >("Height", imageSize[1]);
			dataSet->set< unsigned int >("Depth", imageSize[2]);

			if(!tlp::importGraph("Grid 3D", *dataSet, pluginProgress, graph))
				throw std::runtime_error("Unable to create the grid");

			if(pluginProgress)
				pluginProgress->setComment("Loading the image");

			switch(this->property_type) {
				case COLOR: {
					tlp::ColorProperty *p = graph->getProperty< tlp::ColorProperty >(this->property_name);
					importColor< ImageType >(image, graph, p, convert_to_grayscale, pluginProgress);
				} break;
				case INTEGER: {
					tlp::IntegerProperty *p = graph->getProperty< tlp::IntegerProperty >(this->property_name);
					importData< ImageType, tlp::IntegerProperty, int >(image, graph, p, pluginProgress);
				} break;
				case DOUBLE: {
					tlp::DoubleProperty *p = graph->getProperty< tlp::DoubleProperty >(this->property_name);
					importData< ImageType, tlp::DoubleProperty, double >(image, graph, p, pluginProgress);
				} break;
				case BOOLEAN: {
					tlp::BooleanProperty *p = graph->getProperty< tlp::BooleanProperty >(this->property_name);
					importSelection< ImageType >(image, graph, p, pluginProgress);
				} break;
				case INTEGERVECTOR: {
					tlp::IntegerVectorProperty *p = graph->getProperty< tlp::IntegerVectorProperty >(this->property_name);
					importVectorData< ImageType, tlp::IntegerVectorProperty, int >(image, graph, p, pluginProgress);
				} break;
				case DOUBLEVECTOR: {
					tlp::DoubleVectorProperty *p = graph->getProperty< tlp::DoubleVectorProperty >(this->property_name);
					importVectorData< ImageType, tlp::DoubleVectorProperty, double >(image, graph, p, pluginProgress);
				} break;
			}
		} catch(std::runtime_error &ex) {
			if(pluginProgress)
				pluginProgress->setError(ex.what());
			return false;
		}

		return true;
	}
};

PLUGIN(ImportImage);
