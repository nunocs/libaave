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
 *   demo-qvtk/demo.cxx: QVTK initialization.
 */

#include "demo.h"
#include "global.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkQtTableView.h>
#include <vtkSmartPointer.h>
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkVRMLImporter.h"
#include "vtkPlane.h"
#include "vtkClipPolyData.h"
#include "vtkProperty.h"
#include "vtkOBJReader.h"
#include "vtkPointData.h"
#include "vtkJPEGReader.h"
#include "vtkTexture.h"
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkLineSource.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkLight.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>

#include <vtkEventQtSlotConnect.h>

#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//Initialize listener orientation and source positions. */
double globalPosx=0;
double globalPosy=0;
double globalPosz=2;
double globalFocx=0.1;
double globalFocy=0;
double globalFocz=2;
volatile double globalYawangle=0;
volatile double globalPitchangle=0;
volatile double globalRollangle=0;
double lastGlobalYawangle=0;
double lastGlobalPitchangle=0;
double lastGlobalRollangle=0;
double globalYawOrientation=0;

double yawOffset=0;
double pitchOffset=0;
double rollOffset=0;

double initSourcePosition1[3] = {-9,0,2}; //Blue sphere
//double initSourcePosition2[3] = {0,9,2};
//double initSourcePosition3[3] = {-9,0,2};
//double initSourcePosition4[3] = {0,-9,2};

bool leftFlag = false;
bool rightFlag = false;
bool forwardFlag  = false;
bool backwardFlag = false;
bool lookLeftFlag = false;
bool lookRightFlag = false;
bool lookUpFlag   = false;
bool lookDownFlag = false;
bool initFlag = true;
bool initMovementFlag = true;

vtkRenderer *renderer = vtkRenderer::New();

/* Renderer update rate */
double timerPeriod = 60;

/* Listener movement velocity */
double motionstep = 5/timerPeriod; // 1 m/sec

//Define world boundaries
//double xBoundaries[2] = {-10,10};
//double yBoundaries[2] = {-10,10};

void printWorldCoord() {
    printf("\nGlobal Position    = %f %f %f",globalPosx,globalPosy,globalPosz);
    printf("\nGlobal Focal Point = %f %f %f",globalFocx,globalFocy,globalFocz);
    printf("\nGlobal Yaw Angle   = %f",globalYawangle);
    printf("\nGlobal Pitch Angle = %f",globalPitchangle);
    printf("\nGlobal Pitch Angle = %f",globalRollangle);
}

void setGlobalFocalPoints(double focx,double focy,double focz) {

    globalFocx = focx;
    globalFocy = focy;
    globalFocz = focz;
}

void moveForward() {

//    if ((globalFocx > xBoundaries[0]) & (globalFocx < xBoundaries[1]) & (globalFocy < yBoundaries[0]) & (globalFocy > yBoundaries[1])) {
        globalPosx=globalFocx;
        globalPosy=globalFocy;
        globalFocx+=motionstep*cos(globalYawangle * M_PI/180.0);
        globalFocy-=motionstep*sin(globalYawangle * M_PI/180.0);
//    }
}

void moveBackward() {

//    if ((globalPosx > xBoundaries[0]) & (globalPosx < xBoundaries[1]) & (globalPosy < yBoundaries[0]) & (globalPosy > yBoundaries[1])) {
        globalFocx=globalPosx;
        globalFocy=globalPosy;
        globalPosx-=motionstep*cos(globalYawangle * M_PI/180.0);
        globalPosy+=motionstep*sin(globalYawangle * M_PI/180.0);
//    }
}

void moveLeft() {

//    if ((globalPosx > xBoundaries[0]) & (globalPosx < xBoundaries[1]) & (globalPosy < yBoundaries[0]) & (globalPosy > yBoundaries[1])) {
        globalFocx+=motionstep*sin(globalYawangle * M_PI/180.0);
        globalFocy+=motionstep*cos(globalYawangle * M_PI/180.0);
        globalPosx+=motionstep*sin(globalYawangle * M_PI/180.0);
        globalPosy+=motionstep*cos(globalYawangle * M_PI/180.0);
//    }
}

void moveRight() {

//    if ((globalPosx > xBoundaries[0]) & (globalPosx < xBoundaries[1]) & (globalPosy < yBoundaries[0]) & (globalPosy > yBoundaries[1])) {
        globalFocx-=motionstep*sin(globalYawangle * M_PI/180.0);
        globalFocy-=motionstep*cos(globalYawangle * M_PI/180.0);
        globalPosx-=motionstep*sin(globalYawangle * M_PI/180.0);
        globalPosy-=motionstep*cos(globalYawangle * M_PI/180.0);
//    }
}

void lookUp() {
    globalPitchangle+=0.5;
}

void lookDown() {
    globalPitchangle-=0.5;
}

void lookLeft() {
    globalYawangle-=0.7;
    if (globalYawangle==-180) globalYawangle=180;
}

void lookRight() {
    globalYawangle+=0.7;
    if (globalYawangle==180) globalYawangle=-180;
}

void move() {
    if (forwardFlag)  moveForward();
    if (backwardFlag) moveBackward();
    if (lookLeftFlag) lookLeft();
    if (lookRightFlag) lookRight();
    if (lookUpFlag) lookUp();
    if (lookDownFlag) lookDown();
    if (leftFlag) moveLeft();
    if (rightFlag) moveRight();
}

class vtkKeyPressCallback : public vtkCommand
{
public:
    vtkRenderWindowInteractor *iren;
    int* mouseCoord;
    int* screenSize;
    char* key;

    static vtkKeyPressCallback *New() {
        return new vtkKeyPressCallback;
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        this->key=iren->GetKeySym();
//        printf("\nPressed key = %s\n",this->key);

        if ((std::string)(iren->GetKeySym()) == "Up")  //andar frente
        {
            forwardFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "Down")  //andar trás
        {
            backwardFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "Left")  //andar frente
        {
            leftFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "Right")  //andar trás
        {
            rightFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "g")  //olhar esquerda
        {
            lookLeftFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "j")  //olhar direita
        {
            lookRightFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "y")  //olhar cima
        {
            lookUpFlag = true;
        }
        if ((std::string)(iren->GetKeySym()) == "h")  //olhar baixo
        {
            lookDownFlag = true;
        }
    }
};


class vtkKeyReleaseCallback : public vtkCommand
{
public:
    vtkRenderWindowInteractor *iren;
    char* key;

    static vtkKeyReleaseCallback *New() {
        return new vtkKeyReleaseCallback;
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        this->key=iren->GetKeySym();
//        printf("\nReleased key = %s\n",this->key);

        if ((std::string)(iren->GetKeySym()) == "Up")  //andar frente
        {
            forwardFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "Down")  //andar trás
        {
            backwardFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "Left")  //andar esquerda
        {
            leftFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "Right")  //andar direita
        {
            rightFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "g")  //olhar esquerda
        {
            lookLeftFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "j")  //olhar direita
        {
            lookRightFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "y")  //olhar cima
        {
            lookUpFlag = false;
        }
        if ((std::string)(iren->GetKeySym()) == "h")  //olhar baixo
        {
            lookDownFlag = false;
        }
    }
};

class vtkTimerCallback : public vtkCommand
{
public:
    vtkRenderWindowInteractor *iren;

    static vtkTimerCallback *New()
    {
        vtkTimerCallback *cb = new vtkTimerCallback;
        cb->TimerCount = 0;
        return cb;
    }

    virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId,
                         void *vtkNotUsed(callData))
    {
        if (vtkCommand::TimerEvent == eventId)
        {

            renderer->GetActiveCamera()->Yaw(lastGlobalYawangle-globalYawangle);
            renderer->GetActiveCamera()->Pitch(-(lastGlobalPitchangle-globalPitchangle));
            renderer->GetActiveCamera()->Roll(lastGlobalRollangle-globalRollangle);
            setGlobalFocalPoints(renderer->GetActiveCamera()->GetFocalPoint()[0],renderer->GetActiveCamera()->GetFocalPoint()[1],renderer->GetActiveCamera()->GetFocalPoint()[2]);

            lastGlobalYawangle = globalYawangle;
            lastGlobalPitchangle = globalPitchangle;
            lastGlobalRollangle = globalRollangle;

            move();

            renderer->GetActiveCamera()->SetPosition(globalPosx,globalPosy,globalPosz);
            renderer->GetActiveCamera()->SetFocalPoint(globalFocx,globalFocy,globalFocz);

            //printWorldCoord();

            aave_set_listener_position(aave,globalPosx,globalPosy,globalPosz);
            aave_set_listener_orientation(aave, 0,globalPitchangle * M_PI/180.0,globalYawangle * M_PI/180.0);
            aave_update(aave);

            iren->Render();
        }
    }

private:
    int TimerCount;
    double listenerPos[3];
    double listenerOrient[3];
};

// Constructor
RenderWindowsQt::RenderWindowsQt()
{
    this->setupUi(this);

    //Initialize global renderer
    renderer = vtkRenderer::New();

    //Read .obj data
    vtkOBJReader *myReader = vtkOBJReader::New();
    myReader->SetFileName("../geometry/church.obj");
    myReader->Update();
    myReader->GetOutput()->GetPointData()->SetNormals(NULL);

    //View clipped area
    vtkPolyDataMapper *clipMapper = vtkPolyDataMapper::New();
    clipMapper->SetInput(myReader->GetOutput());
    vtkActor *clipActor = vtkActor::New();
    clipActor->SetMapper(clipMapper);

    //Set initial camera position
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
    camera->SetPosition(globalPosx,globalPosy,globalPosz);
    camera->SetFocalPoint(globalFocx,globalFocy,globalFocz);
    camera->SetViewUp(0,0,1);

    //Set scene lights
    vtkLight *light1 = vtkLight::New();
    light1->SetColor(255,255,255);
    light1->SetPosition(2000,500,-1000);
    light1->SetFocalPoint(2000,501,-1000);
    renderer->AddLight(light1);

    vtkLight *light2 = vtkLight::New();
    light2->SetColor(255,255,255);
    light2->SetPosition(2000,500,-1000);
    light2->SetFocalPoint(2000,499,-1000);
    renderer->AddLight(light2);

    vtkLight *light3 = vtkLight::New();
    light3->SetColor(255,255,255);
    light3->SetPosition(2000,500,-1000);
    light3->SetFocalPoint(2001,500,-1000);
    renderer->AddLight(light3);

    vtkLight *light4 = vtkLight::New();
    light4->SetColor(255,255,255);
    light4->SetPosition(2000,500,-1000);
    light4->SetFocalPoint(1999,499,-1000);
    renderer->AddLight(light4);

    vtkLight *light5 = vtkLight::New();
    light5->SetColor(255,255,255);
    light5->SetPosition(2000,500,-1000);
    light5->SetFocalPoint(1999,499,-1001);
    renderer->AddLight(light5);

    vtkLight *light6 = vtkLight::New();
    light6->SetColor(255,255,255);
    light6->SetPosition(2000,500,-1000);
    light6->SetFocalPoint(1999,499,-999);
    renderer->AddLight(light6);

    //Set source geometry:
    //Source 1 -> Blue sphere
    vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource1->SetRadius(0.05);
    sphereSource1->SetCenter(initSourcePosition1);
    sphereSource1->Update();
    vtkSmartPointer<vtkPolyDataMapper> sphereMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper1->SetInputConnection(sphereSource1->GetOutputPort());
    vtkSmartPointer<vtkActor> sphereActor1 = vtkSmartPointer<vtkActor>::New();
    sphereActor1->SetMapper(sphereMapper1);
    sphereActor1->GetProperty()->SetColor(0,0,1);

    //Source 2 -> Red sphere
//    vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
//    sphereSource2->SetRadius(0.05);
//    sphereSource2->SetCenter(initSourcePosition2);
//    sphereSource2->Update();
//    vtkSmartPointer<vtkPolyDataMapper> sphereMapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
//    sphereMapper2->SetInputConnection(sphereSource2->GetOutputPort());
//    vtkSmartPointer<vtkActor> sphereActor2 = vtkSmartPointer<vtkActor>::New();
//    sphereActor2->SetMapper(sphereMapper2);
//    sphereActor2->GetProperty()->SetColor(1,0,0);
    //Source 3 -> Red sphere
//    vtkSmartPointer<vtkSphereSource> sphereSource3 = vtkSmartPointer<vtkSphereSource>::New();
//    sphereSource3->SetRadius(0.05);
//    sphereSource3->SetCenter(initSourcePosition3);
//    sphereSource3->Update();
//    vtkSmartPointer<vtkPolyDataMapper> sphereMapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
//    sphereMapper3->SetInputConnection(sphereSource3->GetOutputPort());
//    vtkSmartPointer<vtkActor> sphereActor3 = vtkSmartPointer<vtkActor>::New();
//    sphereActor3->SetMapper(sphereMapper3);
//    sphereActor3->GetProperty()->SetColor(0,1,0);
    //Source 4 -> Red sphere
//    vtkSmartPointer<vtkSphereSource> sphereSource4 = vtkSmartPointer<vtkSphereSource>::New();
//    sphereSource4->SetRadius(0.05);
//    sphereSource4->SetCenter(initSourcePosition4);
//    sphereSource4->Update();
//    vtkSmartPointer<vtkPolyDataMapper> sphereMapper4 = vtkSmartPointer<vtkPolyDataMapper>::New();
//    sphereMapper4->SetInputConnection(sphereSource4->GetOutputPort());
//    vtkSmartPointer<vtkActor> sphereActor4 = vtkSmartPointer<vtkActor>::New();
//    sphereActor4->SetMapper(sphereMapper4);
//    sphereActor4->GetProperty()->SetColor(1,1,1);

    //Read and set texture
    vtkJPEGReader* t1 = vtkJPEGReader::New();
    t1->SetFileName("../textures/church_texture.jpg");
    vtkTexture *texture1 = vtkTexture::New();
    texture1->SetInput((vtkDataObject*) t1->GetOutput());
    texture1->InterpolateOn();
    clipActor->SetTexture(texture1);

    //Add actors to renderer
    renderer->AddActor(sphereActor1);
//    renderer->AddActor(sphereActor2);
//    renderer->AddActor(sphereActor3);
//    renderer->AddActor(sphereActor4);
    renderer->AddActor(clipActor);
    renderer->SetBackground(0,0,0); // Background color
    //renderer->ResetCameraClippingRange();
    renderer->GetActiveCamera()->SetClippingRange(0.001,100);
    renderer->GetActiveCamera()->SetPosition(globalPosx,globalPosy,globalPosz);
    renderer->GetActiveCamera()->SetFocalPoint(globalFocx,globalFocy,globalFocz);

    //Initialize keypress callback
    vtkKeyPressCallback *keypressCallback = vtkKeyPressCallback::New();
    keypressCallback->iren=this->qvtkWidget->GetInteractor();

    //Initialize keyrelease callback
    vtkKeyReleaseCallback *keyreleaseCallback = vtkKeyReleaseCallback::New();
    keyreleaseCallback->iren=this->qvtkWidget->GetInteractor();

    //Initialize timer callback
    vtkSmartPointer<vtkTimerCallback> cb = vtkSmartPointer<vtkTimerCallback>::New();
    cb->iren = this->qvtkWidget->GetInteractor();

    //Add observers to the qvtkinteractor
    QVTKInteractor *qviren = this->qvtkWidget->GetInteractor();
    qviren->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);
    qviren->AddObserver(vtkCommand::KeyReleaseEvent, keyreleaseCallback);
    qviren->AddObserver(vtkCommand::TimerEvent, cb);
    int timerId = qviren->CreateRepeatingTimer(timerPeriod);

    this->setCentralWidget(this->qvtkWidget);
    // VTK/Qt wedded
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
}
