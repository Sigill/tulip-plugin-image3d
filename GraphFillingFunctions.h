#ifndef GRAPHFILLINGFUNCTIONS_H
#define GRAPHFILLINGFUNCTIONS_H

#include <itkImageRegionConstIterator.h>
#include <vector>

template <typename TImageType, typename TPropertyType>
void importFeature(TImageType *image, tlp::Graph *graph, TPropertyType *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	{
		int width = 0, height = 0, depth = 0;

		if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
			throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

		if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
			throw std::runtime_error("The dimensions of the graph and the image do not match");
	}


	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		property->setNodeValue(n, (typename TImageType::PixelType)(iterator.Get()));

		++i;
		if(pluginProgress && (i % 10 == 0))
			pluginProgress->progress(i, numberOfPixels);

		++iterator;
	}
}

template <typename TVectorImageType, typename TPropertyType>
void importVectorFeature(TVectorImageType *image, tlp::Graph *graph, TPropertyType *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	{
		int width = 0, height = 0, depth = 0;

		if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
			throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

		if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
			throw std::runtime_error("The dimensions of the graph and the image do not match");
	}


	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();
	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *feature_tmp;
	std::vector< typename TVectorImageType::InternalPixelType > feature(numberOfComponents);
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		feature_tmp = iterator.Get().GetDataPointer();
		feature.assign(feature_tmp, feature_tmp + numberOfComponents);
		property->setNodeValue(n, feature);

		++i;
		if(pluginProgress && (i % 10 == 0))
			pluginProgress->progress(i, numberOfPixels);

		++iterator;
	}
}

template <typename TVectorImageType>
void importColor(TVectorImageType *image, tlp::Graph *graph, tlp::ColorProperty *property, const bool convert_to_grayscale, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TVectorImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	{
		int width = 0, height = 0, depth = 0;

		if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
			throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

		if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
			throw std::runtime_error("The dimensions of the graph and the image do not match");
	}

	const unsigned int numberOfComponents = image->GetNumberOfComponentsPerPixel();

	if(numberOfComponents != 3 && numberOfComponents != 1)
		throw std::runtime_error("The image must have either 1 or 3 channels");

	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	const typename TVectorImageType::InternalPixelType *feature_tmp;
	tlp::Color c;
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TVectorImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		feature_tmp = iterator.Get().GetDataPointer();
		if(numberOfComponents == 1) {
			c.set(feature_tmp[0], feature_tmp[0], feature_tmp[0]);
		} else if(convert_to_grayscale) {
			unsigned char g = (feature_tmp[0] + feature_tmp[1] + feature_tmp[2]) / 3;
			c.set(g, g, g);
		} else {
			c.set(feature_tmp[0], feature_tmp[1], feature_tmp[2]);
		}
		property->setNodeValue(n, c);

		++i;
		if(pluginProgress && (i % 10 == 0))
			pluginProgress->progress(i, numberOfPixels);

		++iterator;
	}
}

template <typename TImageType>
void importSelection(TImageType *image, tlp::Graph *graph, tlp::BooleanProperty *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	{
		int width = 0, height = 0, depth = 0;

		if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
			throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

		if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
			throw std::runtime_error("The dimensions of the graph and the image do not match");
	}


	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		property->setNodeValue(n, (typename TImageType::PixelType)(iterator.Get() > 0));

		++i;
		if(pluginProgress && (i % 10 == 0))
			pluginProgress->progress(i, numberOfPixels);

		++iterator;
	}
}

template <typename TImageType, typename TPropertyType>
void importSelectionAsNumericProperty(TImageType *image, tlp::Graph *graph, TPropertyType *property, tlp::PluginProgress *pluginProgress = NULL)
{
	const typename TImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();

	{
		int width = 0, height = 0, depth = 0;

		if(!(graph->getAttribute<int>("width", width) && graph->getAttribute<int>("height", height) && graph->getAttribute<int>("depth", depth)))
			throw std::runtime_error("Unable to get the image dimensions from the graph. Make sure it has been created by the \"Image 3D\" import plugin");

		if((unsigned long)width != imageSize[0] || (unsigned long)height != imageSize[1] || (unsigned long)depth != imageSize[2])
			throw std::runtime_error("The dimensions of the graph and the image do not match");
	}


	const unsigned int numberOfPixels = imageSize[0] * imageSize[1] * imageSize[2];
	tlp::node n;
	int i = 0;

	itk::ImageRegionConstIterator< TImageType > iterator(image, image->GetLargestPossibleRegion());
	forEach(n, graph->getNodes())
	{
		property->setNodeValue(n, (typename TImageType::PixelType)(iterator.Get() > 0 ? 1 : 0));

		++i;
		if(pluginProgress && (i % 10 == 0))
			pluginProgress->progress(i, numberOfPixels);

		++iterator;
	}
}

#endif /* GRAPHFILLINGFUNCTIONS_H */
