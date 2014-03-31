#include <QApplication>
#include <QKeyEvent>
#include <QWidget>

#include <unistd.h>

#include "demo.h"
#include "imu.h"
#include "audio.h"
#include "global.h"

#define VOLUME 1600
#define AREA 1120
#define RT60 2110

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
  aave_read_obj(aave, "../Geometry/shoebox_aave.obj");

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

  //isense initialization
  //new Imu();

  sleep(1);

  RenderWindowsQt RenderWindowsQt;
  RenderWindowsQt.show();//FullScreen();

  return app.exec();
}
