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

### Image import plugin

Name: **Import image**.

Parameters:
* **file::Image**: String, the path of the source image.
* **Neighborhood type**: StringCollection, the type of neighborhood to build (Circular, Square).
* **Neighborhood radius**: Double, the radius of the neighborhood.
* **Positionning**: Boolean, indicates if the nodes should be positionned in space.
* **Spacing**: Double, the space between each nodes.
* **Property type**: StringCollection, the type of the property that will store the pixel's data. Color, Integer, Double, IntegerVector, DoubleVector or Boolean.
* **Property name**: String, the name of the property to create.
* **Convert to grayscale**: Boolean, indicates if a Color property should be converted to grayscale.

### Image loading plugin

Name: **Load image data**.

Parameters:
* **file::Image**: The path of the source image.
* **Property**: The property to use.
* **Convert to grayscale**: Boolean, indicates if a Color property should be converted to grayscale.

## Export image plugin

Name: **Export image**.

Parameters:
* **Property**: The property to export (Color or Boolean).
* **dir::Export directory**: The directory in which tthe image(s) will be created.
* **Export pattern**: The pattern that will be used to create the filenames. For image formats that doesn't support 3D, use printf-like tokens to specify a numerical index ("%06d").

## LICENSE

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

