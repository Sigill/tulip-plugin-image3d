#include <tulip/TulipPluginHeaders.h>
#include <tulip/StringCollection.h>
#include <math.h>
#include <stdexcept>

#include <itkRGBPixel.h>
#include <itkImage.h>
#include <itkImageSeriesWriter.h>
#include <itkImageRegionIterator.h>
#include <itkNumericSeriesFileNames.h>

#include <QDir>

#include "GraphFillingFunctions2.h"

typedef unsigned char UCPixelType;
typedef itk::RGBPixel< unsigned char > RGBPixelType;

typedef itk::Image< UCPixelType, 3 > UCImageType;
typedef itk::Image< RGBPixelType, 3 > RGBImageType;

typedef typename itk::ImageSeriesWriter< UCImageType, itk::Image< UCPixelType, 2 > > UCImageSeriesWriterType;
typedef typename itk::ImageSeriesWriter< RGBImageType, itk::Image< RGBPixelType, 2 > > RGBImageSeriesWriterType;

using namespace std;
using namespace tlp;

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
			throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

const string PLUGIN_NAME("Export image");

namespace {
const char *paramHelp[] = {
	// 0 Property
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Property")
		HTML_HELP_DEF("Default", "data")
		HTML_HELP_BODY()
		"The Property you want to export (Color, Boolean)."
		HTML_HELP_CLOSE(),

	// 1 Export directory
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "Directory")
		HTML_HELP_BODY()
		"Path of the image that will be created."
		HTML_HELP_CLOSE(),

	// 3 Export pattern
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "String")
		HTML_HELP_BODY()
		"Name of image that will be created."
		HTML_HELP_CLOSE()
};
}

class ExportImage: public tlp::Algorithm {
private:
	tlp::PropertyInterface *property;
	std::string export_dir, export_pattern;
	tlp::StringCollection pixel_format;

	int height, width, depth;

	enum property_t { COLOR, BOOLEAN };
	property_t property_type;

public:
	PLUGININFORMATIONS(PLUGIN_NAME, "Cyrille Faucheux", "2013-08-20", "", "1.0", "File")

	/*
	std::string category() const {
		return EXPORT_CATEGORY;
	}

	std::string icon() const {
		return ":/tulip/gui/icons/32/plugin_import_export.png";
	}
	*/

	ExportImage(PluginContext *context) :
		tlp::Algorithm(context)
	{
		addInParameter< tlp::PropertyInterface* > ("Property",              paramHelp[0], "data");
		addInParameter< std::string >             ("dir::Export directory", paramHelp[1], "");
		addInParameter< std::string >             ("Export pattern",        paramHelp[1], "out.bmp");
	}

	~ExportImage() {}

	bool check(std::string &err) {
		try {
			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided.");

			CHECK_PROP_PROVIDED("Property", property);
			CHECK_PROP_PROVIDED("dir::Export directory", export_dir);
			CHECK_PROP_PROVIDED("Export pattern", export_pattern);

			if(!(graph->getAttribute<int>("width", this->width) && graph->getAttribute<int>("height", this->height) && graph->getAttribute<int>("depth", this->depth)))
				throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

			if(export_dir.empty()) {
				std::stringstream e; e << "The \"file::Export directory\" parameter cannot be empty";
				throw std::runtime_error(e.str());
			}

			if(export_pattern.empty()) {
				std::stringstream e; e << "The \"Export pattern\" parameter cannot be empty";
				throw std::runtime_error(e.str());
			}

			if (dynamic_cast< tlp::ColorProperty* >(this->property)) {
				this->property_type = COLOR;
			} else if (dynamic_cast< tlp::BooleanProperty* >(this->property)) {
				this->property_type = BOOLEAN;
			} else {
				throw std::runtime_error("\"Property\" must be a property of one of the following types: "
				                         "ColorProperty, BooleanProperty.");
			}

		} catch (std::runtime_error &ex) {
			err.assign(ex.what());
			return false;
		}

		return true;
	}

	bool run()
	{
		try {
			const unsigned int numberOfPixels = this->width * this->height * this->depth;
			int i = 0;
			std::string out = QDir(QString(export_dir.c_str())).filePath(export_pattern.c_str()).toStdString();
			itk::NumericSeriesFileNames::Pointer filenameGenerator = itk::NumericSeriesFileNames::New();
			filenameGenerator->SetStartIndex(0);
			filenameGenerator->SetEndIndex(this->depth - 1);
			filenameGenerator->SetIncrementIndex(1);
			filenameGenerator->SetSeriesFormat(out);

			switch(this->property_type) {
				case COLOR: {
					RGBImageType::Pointer image = RGBImageType::New();

					RGBImageType::IndexType origin = {{0, 0, 0}};
					RGBImageType::SizeType size;
					size[0] = width; size[1] = height; size[2] = depth;

					RGBImageType::RegionType region(origin, size);
					image->SetRegions(region);
					image->Allocate();

					tlp::ColorProperty *prop = dynamic_cast< tlp::ColorProperty* >(this->property);
					itk::ImageRegionIterator< RGBImageType > iterator(image, image->GetLargestPossibleRegion());
					tlp::node n;
					forEach(n, graph->getNodes())
					{
						tlp::Color gc = prop->getNodeValue(n);
						RGBPixelType pix;
						pix.SetRed(gc.getR());
						pix.SetGreen(gc.getG());
						pix.SetBlue(gc.getB());

						iterator.Set(pix);

						++i;
						++iterator;

						reportProgress(pluginProgress, i, numberOfPixels);
					}

					try {
						typename RGBImageSeriesWriterType::Pointer writer = RGBImageSeriesWriterType::New();
						writer->SetInput(image);
						writer->SetFileNames(filenameGenerator->GetFileNames());
						writer->Update();
					} catch ( itk::ExceptionObject & err ) {
						std::stringstream e; e << "The image cannot be exported: " << err.GetDescription();
						throw std::runtime_error(e.str());
					}
				} break;
				case BOOLEAN: {
					  UCImageType::Pointer image = UCImageType::New();

					UCImageType::IndexType origin = {{0, 0, 0}};
					UCImageType::SizeType size;
					size[0] = width; size[1] = height; size[2] = depth;

					UCImageType::RegionType region(origin, size);
					image->SetRegions(region);
					image->Allocate();

					tlp::BooleanProperty *prop = dynamic_cast< tlp::BooleanProperty* >(this->property);
					itk::ImageRegionIterator< UCImageType > iterator(image, image->GetLargestPossibleRegion());
					tlp::node n;
					forEach(n, graph->getNodes())
					{
						iterator.Set(prop->getNodeValue(n) ? 255 : 0);

						++i;
						++iterator;

						reportProgress(pluginProgress, i, numberOfPixels);
					}

					try {
						typename UCImageSeriesWriterType::Pointer writer = UCImageSeriesWriterType::New();
						writer->SetInput(image);
						writer->SetFileNames(filenameGenerator->GetFileNames());
						writer->Update();
					} catch ( itk::ExceptionObject & err ) {
						std::stringstream e; e << "The image cannot be exported: " << err.GetDescription();
						throw std::runtime_error(e.str());
					}
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

PLUGIN(ExportImage);
