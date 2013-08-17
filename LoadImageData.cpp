#include <tulip/TulipPluginHeaders.h>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

#include "PluginUtils.h"
#include "GraphFillingFunctions2.h"

#include <sstream>
#include <stdexcept>

typedef itk::VectorImage< double, 3 > ImageType;
typedef typename itk::ImageFileReader< ImageType > ImageReaderType;

namespace {
const char* paramHelp[] = {
	// 0 File name
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "File")
		HTML_HELP_BODY()
		"The image to import."
		HTML_HELP_CLOSE(),
	
	// 1 Property
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Property")
		HTML_HELP_DEF("Default", "data")
		HTML_HELP_BODY()
		"The Property where the data will be stored."
		HTML_HELP_CLOSE(),

	// 2 Convert to grayscale
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Boolean")
		HTML_HELP_DEF("Default", "false")
		HTML_HELP_BODY()
		"Indicates if the color should be converted to grayscale."
		HTML_HELP_CLOSE(),
};
}

class LoadImageData: public tlp::Algorithm {
private:
	std::string file;
	tlp::PropertyInterface *property;
	bool convert_to_grayscale;

	enum property_t { COLOR, INTEGER, DOUBLE, INTEGERVECTOR, DOUBLEVECTOR, BOOLEAN };
	property_t property_type;

	typename ImageReaderType::Pointer imageReader;

public:
	PLUGININFORMATIONS("Load image data", "Cyrille FAUCHEUX", "2013-08-18", "", "1.0", "Image")

	LoadImageData(const tlp::PluginContext *context):
		tlp::Algorithm(context)
	{
		addInParameter< std::string >            ("file::Image",           paramHelp[0], "");
		addInParameter< tlp::PropertyInterface* >("Property",              paramHelp[1], "data");
		addInParameter< bool >                   ("Convert to grayscale",  paramHelp[2], "false", false);
	}

	~LoadImageData() {}

	bool check(std::string &err) {
		try {
			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided.");

			CHECK_PROP_PROVIDED("file::Image", this->file);
			CHECK_PROP_PROVIDED("Property", this->property);

			if (dynamic_cast< tlp::ColorProperty* >(this->property)) {
				this->property_type = COLOR;
				CHECK_PROP_PROVIDED("Convert to grayscale", this->convert_to_grayscale);
			} else if (dynamic_cast< tlp::IntegerProperty* >(this->property)) {
				this->property_type = INTEGER;
			} else if (dynamic_cast< tlp::IntegerVectorProperty* >(this->property)) {
				this->property_type = INTEGERVECTOR;
			} else if (dynamic_cast< tlp::DoubleProperty* >(this->property)) {
				this->property_type = DOUBLE;
			} else if (dynamic_cast< tlp::DoubleVectorProperty* >(this->property)) {
				this->property_type = DOUBLEVECTOR;
			} else if (dynamic_cast< tlp::BooleanProperty* >(this->property)) {
				this->property_type = BOOLEAN;
			} else {
				throw std::runtime_error("\"Property\" must be a property of one of the following types: "
				                         "ColorProperty, IntegerProperty, DoubleProperty, IntegerVectorProperty, DoubleVectorProperty, BooleanProperty.");
			}

			if(file.empty()) {
				std::stringstream e; e << "The \"File\" parameter cannot be empty";
				throw std::runtime_error(e.str());
			}

			imageReader = ImageReaderType::New();
			imageReader->SetFileName(file);
			try {
				imageReader->Update();
			} catch ( itk::ExceptionObject &err ) {
				std::stringstream e; e << "The image located at \"" << file << "\" is not readable";
				throw std::runtime_error(e.str());
			}

			int width = 0, height = 0, depth = 0;
			typename ImageType::Pointer image = imageReader->GetOutput();
			typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

			if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
				throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

			if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
				throw std::runtime_error("The dimensions of the graph and the image do not match");

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

		} catch (std::runtime_error &ex) {
			err.assign(ex.what());
			return false;
		}

		return true;
	}

	bool run() {
		try {
			typename ImageType::Pointer image = imageReader->GetOutput();
			typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

			if(pluginProgress)
				pluginProgress->setComment("Loading the image");

			switch(this->property_type) {
				case COLOR: {
					tlp::ColorProperty* p = dynamic_cast< tlp::ColorProperty* >(this->property);
					importColor< ImageType >(image, graph, p, convert_to_grayscale, pluginProgress);
				} break;
				case INTEGER: {
					tlp::IntegerProperty* p = dynamic_cast< tlp::IntegerProperty* >(this->property);
					importData< ImageType, tlp::IntegerProperty, int >(image, graph, p, pluginProgress);
				} break;
				case DOUBLE: {
					tlp::DoubleProperty* p = dynamic_cast< tlp::DoubleProperty* >(this->property);
					importData< ImageType, tlp::DoubleProperty, double >(image, graph, p, pluginProgress);
				} break;
				case BOOLEAN: {
					tlp::BooleanProperty* p = dynamic_cast< tlp::BooleanProperty* >(this->property);
					importSelection< ImageType >(image, graph, p, pluginProgress);
				} break;
				case INTEGERVECTOR: {
					tlp::IntegerVectorProperty* p = dynamic_cast< tlp::IntegerVectorProperty* >(this->property);
					importVectorData< ImageType, tlp::IntegerVectorProperty, int >(image, graph, p, pluginProgress);
				} break;
				case DOUBLEVECTOR: {
					tlp::DoubleVectorProperty* p = dynamic_cast< tlp::DoubleVectorProperty* >(this->property);
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

PLUGIN(LoadImageData)
