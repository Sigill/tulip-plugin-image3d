# Tulip Image3D plugins

## Description

Plugins for the tulip platform to import properties from images. They support 2D and 3D images, with any number of components.

Uses the [ITK](http://www.itk.org/) library to load the images.

Relies on the [Grid3D](http://github.com/Sigill/tulip-plugin-grid3d-import) plugin.

## Build

Launch one of the CMake project configuration tool and select your build directory. Set the CMAKE_MODULE_PATH variable to the location of the FindTULIP.cmake file (should be &lt;tulip_install_dir&gt;/share/tulip), and set the ITK_DIR variable to the location of the ITKConfig.cmake file.

More informations on how to build plugins [here](http://tulip.labri.fr/TulipDrupal/?q=node/1481).

Note: ITK must be built with position independent code (-fpic option for GCC) and as shared libraries. The ITK libraries must be placed in the lib/ folder of Tulip. On Linux, it is also possible to launch Tulip from the command line by modifying the _LD_LIBRARY_PATH_ environment variable:

	LD_LIBRARY_PATH=/path/to/ITK/lib/directory/ ./bin/tulip

## Use

### Image import plugins

* _Import image as color_
* _Import image as double_
* _Import image as integer_
* _Import image as doubleVector_
* _Import image as integerVector_

All plugins uses the following parameters:

* _file::Image_: The path of the source image.
* _Connectivity_: StringCollection indicating the connectivity of the graph ("0", "4" or "8").
* _Positionning_: Boolean indicating if the nodes should be positionned in space.
* _Spacing_: Double indicating the space between each nodes.

The _Import image as color_ plugin will use the _viewColor_ property. For the others, you can specify which property to use with the _Property name_ parameter.

The _Import image as color_ plugin also uses the _Convert to grayscale_ boolean parameter to tell if the image should be converted to grayscale.

### Image loading plugins

* _Load color property from image_
* _Load double property from image_
* _Load integer property from image_
* _Load doubleVector property from image_
* _Load integerVector property from image_
* _Load mask as selection from image_
* _Load mask as double property from image_

All plugins will load the data from an image in a property from an existing graph (created with the Grid3D plugin). All plugins uses the following parameters:

* _file::Image_: The path of the source image.
* _Property_: The property to use.

The _Load color property from image_ also uses the following parameter:

* _Convert to grayscale_: boolean indicating if a color image should be converted to grayscale.

## LICENSE

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

