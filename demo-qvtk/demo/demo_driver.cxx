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
 *
 *   demo-qvtk/demo.cxx: LibAAVE initialization and demo execution.
 */


#include <QApplication>
#include <QKeyEvent>
#include <QWidget>

#include <unistd.h>

#include "demo.h"
#include "imu.h"
#include "audio.h"
#include "global.h"

#define VOLUME 3600
#define AREA 2120
#define RT60 4110

struct aave *aave = (struct aave *)malloc(sizeof *aave);

int main(int argc, char** argv)
{
  // QT Stuff
  QApplication app( argc, argv );

  /* Set auralization input parameters */

  aave->area = AREA;
  aave->volume = VOLUME;
  aave->rt60 = RT60;

  /* Read room model. */
  aave_read_obj(aave, "../geometry/church_aave.obj");

  /* Select the HRTF set to use. */
  aave_hrtf_mit(aave);
  //aave_hrtf_cipic(aave);
  //aave_hrtf_listen(aave);
  //aave_hrtf_tub(aave);

  /* Initialise the auralisation engine. */
  aave_init(aave);

  /* Extra auralization parameters */
  //aave->reverb->active = 1;
  //aave->reverb->mix = 0;
  aave->reflections = 4;

  /* Set initial position and orientation of the listener. */
  aave_set_listener_position(aave,globalPosx,globalPosy,globalPosz);
  aave_set_listener_orientation(aave, 0,0,globalYawangle * M_PI/180.0);

  /* Create one sound source. */
  struct aave_source *source;
  source = (struct aave_source *) malloc(sizeof *source);
  aave_init_source(aave, source);
  aave_add_source(aave, source);
  aave_set_source_position(source,initSourcePosition1[0],initSourcePosition1[1],initSourcePosition1[2]);

  /* Create one sound source. */
//  source = (struct aave_source *)malloc(sizeof *source);
//  aave_init_source(aave, source);
//  aave_add_source(aave, source);
//  aave_set_source_position(source,initSourcePosition2[0],initSourcePosition2[1],initSourcePosition2[2]);

//  /* Create one sound source. */
//  source = (struct aave_source *)malloc(sizeof *source);
//  aave_init_source(aave, source);
//  aave_add_source(aave, source);
//  aave_set_source_position(source,initSourcePosition3[0],initSourcePosition3[1],initSourcePosition3[2]);

//  /* Create one sound source. */
//  source = (struct aave_source *)malloc(sizeof *source);
//  aave_init_source(aave, source);
//  aave_add_source(aave, source);
//  aave_set_source_position(source,initSourcePosition4[0],initSourcePosition4[1],initSourcePosition4[2]);

  /* Update geometric parameters. */
    aave_update(aave);

    new Audio(aave);

  /* isense initialization */
  //new Imu();

  sleep(1);

  RenderWindowsQt RenderWindowsQt;
  RenderWindowsQt.show();//FullScreen();

  return app.exec();
}
