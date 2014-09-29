/*   This file is part of LibAAVE.
 * 
 *   LibAAVE is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibAAVE is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibAAVE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Copyright 2013 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *   Demonstration program of the AcousticAVE library with QT 4 and VTK 5.8 graphics library.
 *
 */

SETUP

1) Copy a room model file, in Wavefront .obj format, to the geometry folder. Load respective texture in the texture folder.

2) Copy one or more anechoic sounds, in 16-bit mono 44100Hz raw format, to the sounds folder. Set their paths in the sources[] array.

3) Initialize volume, area and rt60, room geometry and the hrtf set before calling aave_init().

BUILD

GNU/Linux

Uses the Advanced Linux Sound Architecture library (libasound)
for sound output and thus is compatible with both Qt4 and Qt5.

1) Install libvtk5.8-qt4 package and all associated packages to build qvtk apps.

2) Open CMakeLists.txt:  Edit the path for LibAAVE and isense libraries.

3) Build the program:

- Use CMake to create a makefile.

- Make.

4) If using isense edit isports.ini and edit the name of the device connected to the sensor
Example: Port1 = /dev/ttyUSB0

5) Set the library path to include the isense directory
$ export LD_LIBRARY_PATH=$PWD/isense

6) Run the program
$ ./demo

