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

The _Load*_ plugins will load the data from an image in a property from an existing graph (created with the Grid3D plugin). All plugins uses the following parameters:

* _file::Image_: The path of the source image.
* _Property_: The property to use.

The LoadColorPropertyFromImage also uses the following parameter:

* _Convert to grayscale_: boolean indicating if a color image should be converted to grayscale.

## LICENSE

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

