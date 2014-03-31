//Global variables

#include "vtkRenderer.h"

extern "C" {
#include <../libaave/aave.h>
}

#include <math.h>


/* Source positions */
extern double initSourcePosition1[3];
extern double initSourcePosition2[3];
extern double initSourcePosition3[3];
extern double initSourcePosition4[3];

/* Listener position and orientation variables */
extern double globalPosx;
extern double globalPosy;
extern double globalPosz;
extern double globalFocx;
extern double globalFocy;
extern double globalFocz;

extern volatile double globalRollangle;
extern volatile double globalPitchangle;
extern volatile double globalYawangle;
extern double lastGlobalYawangle;
extern double lastGlobalPitchangle;
extern double lastGlobalRollangle;

extern double yawOffset;
extern double pitchOffset;
extern double rollOffset;

/* Room bounding box */
extern double xBoundaries[2];
extern double yBoundaries[2];

/* Movement indicators */
extern bool leftFlag;
extern bool rightFlag;
extern bool forwardFlag;
extern bool backwardFlag;
extern bool lookLeftFlag;
extern bool lookRightFlag;
extern bool lookUpFlag;
extern bool lookDownFlag;
extern bool initFlag;

// VTK renderer
extern vtkRenderer *renderer;

// The auralisation engine.
extern struct aave *aave;

void printWorldCoord();
void setGlobalFocalPoints(double focx,double focy,double focz);
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void lookUp();
void lookDown();
void lookLeft();
void lookRight();
