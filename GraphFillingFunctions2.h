#ifndef GRAPHFILLINGFUNCTIONS2_H
#define GRAPHFILLINGFUNCTIONS2_H

#include <itkImageRegionConstIterator.h>
#include <vector>

void reportProgress(tlp::PluginProgress *pluginProgress, unsigned int step, unsigned int max)
{
	if(pluginProgress && (step % 10 == 0))
		pluginProgress->progress(step, max);
}

template <typename TVectorImageType>
void importColor(TVectorImageType *image, tlp::Graph *graph, tlp::ColorProperty *property, const bool convert_to_grayscale, tlp::PluginProgress *pluginProgress = NULL)
{
	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();

	if(numberOfComponents != 3 && numberOfComponents != 1)
		throw std::runtime_error("The image must have either 1 or 3 components per pixel.");

	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *data_raw;
	tlp::Color c;
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		data_raw = iterator.Get().GetDataPointer();
		if(numberOfComponents == 1) {
			c.set(data_raw[0], data_raw[0], data_raw[0]);
		} else if(convert_to_grayscale) {
			unsigned char g = (data_raw[0] + data_raw[1] + data_raw[2]) / 3;
			c.set(g, g, g);
		} else {
			c.set(data_raw[0], data_raw[1], data_raw[2]);
		}
		property->setNodeValue(n, c);

		++i;
		reportProgress(pluginProgress, i, numberOfPixels);

		++iterator;
	}
}

template <typename TVectorImageType, typename TPropertyType, typename TValueType>
void importData(TVectorImageType *image, tlp::Graph *graph, TPropertyType *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();

	/* See ITK/BMP bug
	if(1 != numberOfComponents)
		throw std::runtime_error("The image must have either 1 component per pixel.");
	*/

	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *data_raw;
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		data_raw = iterator.Get().GetDataPointer();
		property->setNodeValue(n, (TValueType)(data_raw[0]));

		++i;
		reportProgress(pluginProgress, i, numberOfPixels);

		++iterator;
	}
}

template <typename TVectorImageType, typename TPropertyType, typename TValueType>
void importVectorData(TVectorImageType *image, tlp::Graph *graph, TPropertyType *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();
	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *data_raw;
	std::vector< TValueType > data(numberOfComponents);
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		data_raw = iterator.Get().GetDataPointer();
		for(int j = 0; j < numberOfComponents; ++j)
			data[j] = (TValueType)(data_raw[j]);
		property->setNodeValue(n, data);

		++i;
		reportProgress(pluginProgress, i, numberOfPixels);

		++iterator;
	}
}

template <typename TVectorImageType>
void importSelection(TVectorImageType *image, tlp::Graph *graph, tlp::BooleanProperty *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();

	if(1 != numberOfComponents)
		throw std::runtime_error("The image must have either 1 component per pixel.");

	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *data_raw;
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		data_raw = iterator.Get().GetDataPointer();
		property->setNodeValue(n, data_raw[0] > 0);

		++i;
		reportProgress(pluginProgress, i, numberOfPixels);

		++iterator;
	}
}

#endif /* GRAPHFILLINGFUNCTIONS2_H */
