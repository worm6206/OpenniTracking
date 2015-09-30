#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <OpenNI.h>
#include <nite.h>
#include <iomanip>
#include "Guitar.h"
#include <KinectProperties.h>
#include <fstream>
#include <string>


using namespace openni;
using namespace nite;

float buff;
char buf[10];
int LeftZone,RightZone, MidiCh = 144,MidiVel = 90,MuteMode =0,Exit=0,SetChoice,AngleMove = 0,MidiPort = -1,
	HideConsole =1,ScreenResolutionX =1152,ScreenResolutionY= 864,UserCount =0, UserLimit = 1,ChordNum=0, Note;
Joints Joint[15];
string TextTopLeft = "Waiting for user",TextLeftHand,TextRightHand,TextFPS,TextSettingInfo,Chords[9];
stringstream ss;
long value;
bool OnTheRun=1,ChoiceToBeDone =0,PlayGuitar = 0,initialize = 1,IsBass=0;


void Guitar(){
	//Initialize
	if(initialize){
		buff = Joint[8].y - Joint[7].y;
		
		initialize = 0;
	}

	//Check Exit gesture
	if(Joint[7].y < Joint[0].y && !ChoiceToBeDone && SetChoice==3)OnTheRun = 0;

	//MuteMode check
	if(Joint[7].y < Joint[0].y && !ChoiceToBeDone && SetChoice==1){
		TextTopLeft = "Mute Mode";
		TextLeftHand = "M";
		TextRightHand = "M";
		return;
	}

	//Left hand's x-axis convert to integer number
	TextLeftHand = ZoneSeperate(Joint[6],Joint[8].y,Chords[0],Chords[1],Chords[2],Chords[3],Chords[4],Chords[5],Chords[6],Chords[7]);

	strcpy(buf,TextLeftHand.c_str());
	int current = 0;
	switch(buf[current]){
		case 'C':Note = 1;
			break;
		case 'D':Note = 3;
			break;
		case 'E':Note = 5;
			break;
		case 'F':Note = 6;
			break;
		case 'G':Note = 8;
			break;
		case 'A':Note = 10;
			break;
		case 'B':Note = 12;
			break;
		default:break;}
	current++;
	if(buf[current]=='#'){
		Note++;
		current++;}
	if(buf[current]=='7'){Note += 24;}else if(buf[current]=='m'){				
		if(buf[current+1]=='7'){Note+=36;}
		else{Note+=12;}
	}
	if(buf[current]=='s'){
		if(buf[current+3]=='2'){Note+=48;}
		else if (buf[current+3]=='4'){Note+=60;}
	}
	ChordNum = Note;

	//Right hand's z-axis deciding Midi velocity
	if(Joint[7].z - Joint[3].z<-400){
		MidiVel = 110;
		TextRightHand="3";
	}else if(Joint[7].z - Joint[3].z <-200){
		MidiVel = 80;
		TextRightHand ="2";
	}else{
		MidiVel = 60;
		TextRightHand ="1";
	}

	//Right hand triggering and left hand's x-axis playing chorus
	if(!((buff>0 && (Joint[8].y-Joint[7].y) >0)||(buff<0 && (Joint[8].y-Joint[7].y)<0))){PlayGuitar = 1;}
	buff = Joint[8].y-Joint[7].y;
	if(PlayGuitar && TextLeftHand != "0" && TextLeftHand != ""){
		if(IsBass){PlayBass(ChordNum,MidiCh,MidiVel);}
		else{Play(ChordNum,MidiCh,MidiVel);}
		PlayGuitar = 0;
	}

	//Right hand's y-axis and left hand's x-axis setting Midi channel
	TextSettingInfo ="";
	if(Joint[7].y > Joint[0].y){
		TextLeftHand = "";
		TextRightHand = "";
		if(Joint[6].x < Joint[4].x){
			if(Joint[6].y > Joint[0].y){
				SetChoice = 1;
				TextSettingInfo = "Mute Mode";
			}else if(Joint[6].y > Joint[9].y){
				SetChoice = 2;
				TextSettingInfo = "Channel = 1";
			}else{
				SetChoice = 5;
				TextSettingInfo = "Angle up";
			}
		}
		else if(Joint[6].x > Joint[4].x){
			if(Joint[6].y > Joint[0].y){
				SetChoice = 3;
				TextSettingInfo = "Exit";
			}else if(Joint[6].y > Joint[9].y){
				SetChoice = 4;
				TextSettingInfo = "Channel = 2";
			}else{
				SetChoice = 6;
				TextSettingInfo = "Angle Down";
			}
		}
		TextTopLeft = "Setting Mode";
		ChoiceToBeDone = 1;
	}
	if(Joint[7].y < Joint[0].y &&ChoiceToBeDone){
		switch(SetChoice){
		case 1:
			MuteMode = 1;
			break;
		case 2:
			MuteMode = 0;
			MidiCh = 144;
			break;
		case 3:
			Exit = 1;
			break;
		case 4:
			MuteMode = 0;
			MidiCh = 145;
			break;
		case 5:
			AngleMove = 1;
			break;
		case 6:
			AngleMove = 2;
			break;
		default:
			;
		}
		ChoiceToBeDone = 0;
	}
}

int main( int argc, char **argv )
{
     string line;
  ifstream myfile ("set.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      cout << line << endl;
    }
    myfile.close();
  }

  int i =0;
    stringstream ssin(line);
	while (ssin.good() && i < 9){
        ssin >> Chords[i];
		if(Chords[i]=="Bass" || Chords[i] == "BASS"){
			IsBass = true;
			Chords[i]="0";
		}
        ++i;
    }
	

	OpenNI::initialize();
	Device  mDevice;
	mDevice.open( ANY_DEVICE );
	VideoStream mDepthStream;
	mDepthStream.create( mDevice, SENSOR_DEPTH );
	VideoMode mDMode;
	mDMode.setResolution( 640, 480 );
	mDMode.setFps( 30 );
	mDMode.setPixelFormat( PIXEL_FORMAT_DEPTH_1_MM );
	mDepthStream.setVideoMode( mDMode);
	VideoStream mColorStream;
	mColorStream.create( mDevice, SENSOR_COLOR );
	VideoMode mCMode;
	mCMode.setResolution( 640, 480 );
	mCMode.setFps( 30 );
	mCMode.setPixelFormat( PIXEL_FORMAT_RGB888 );
	mColorStream.setVideoMode( mCMode);
	mDevice.setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR );
	NiTE::initialize();
	initMIDI(MidiPort);
	if(HideConsole){
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_HIDE );
	}
	UserTracker mUserTracker;
	mUserTracker.create( &mDevice );
	mUserTracker.setSkeletonSmoothingFactor( 0.1f );
	cv::namedWindow( "2013Openni2_DEMO",  0 );
	mColorStream.start();
	mDepthStream.start();

	while( OnTheRun )
	{
		cv::Mat cImageBGR;
		VideoFrameRef mColorFrame;
		mColorStream.readFrame( &mColorFrame );
		const cv::Mat mImageRGB( mColorFrame.getHeight(), mColorFrame.getWidth(),
			CV_8UC3, (void*)mColorFrame.getData() );
		cv::cvtColor( mImageRGB, cImageBGR, CV_RGB2BGR );
		UserTrackerFrameRef  mUserFrame;
		mUserTracker.readFrame( &mUserFrame );
		cv::putText( cImageBGR, TextTopLeft, cv::Point( 5, 40 ), cv::FONT_HERSHEY_SIMPLEX,1.3, cv::Scalar( 0, 0, 255 ), 4 );
		const nite::Array<UserData>& aUsers = mUserFrame.getUsers();

		for( int i = 0; i < aUsers.getSize(); ++ i )
		{
			const UserData& rUser = aUsers[i];
			if( rUser.isNew() && UserCount < UserLimit )
				mUserTracker.startSkeletonTracking( rUser.getId() );
			if( rUser.isVisible() )
			{
				TextTopLeft = "";
				const Skeleton& rSkeleton = rUser.getSkeleton();
				if( rSkeleton.getState() == SKELETON_TRACKED )
				{
					Guitar();
					//cout<< Velocity(Velocity(Joint[7].y)) <<endl;
					SkeletonJoint aJoints[15];
					aJoints[ 0] = rSkeleton.getJoint( JOINT_HEAD );
					aJoints[ 1] = rSkeleton.getJoint( JOINT_NECK );
					aJoints[ 2] = rSkeleton.getJoint( JOINT_LEFT_SHOULDER );
					aJoints[ 3] = rSkeleton.getJoint( JOINT_RIGHT_SHOULDER );
					aJoints[ 4] = rSkeleton.getJoint( JOINT_LEFT_ELBOW );
					aJoints[ 5] = rSkeleton.getJoint( JOINT_RIGHT_ELBOW );
					aJoints[ 6] = rSkeleton.getJoint( JOINT_LEFT_HAND );
					aJoints[ 7] = rSkeleton.getJoint( JOINT_RIGHT_HAND );
					aJoints[ 8] = rSkeleton.getJoint( JOINT_TORSO );
					aJoints[ 9] = rSkeleton.getJoint( JOINT_LEFT_HIP );
					aJoints[10] = rSkeleton.getJoint( JOINT_RIGHT_HIP );
					//aJoints[11] = rSkeleton.getJoint( JOINT_LEFT_KNEE );
					//aJoints[12] = rSkeleton.getJoint( JOINT_RIGHT_KNEE );
					//aJoints[13] = rSkeleton.getJoint( JOINT_LEFT_FOOT );
					//aJoints[14] = rSkeleton.getJoint( JOINT_RIGHT_FOOT );

					cv::Point2f aPoint[11];
					for( int  s = 0; s < 11; ++ s )
					{
						const Point3f& rPos = aJoints[s].getPosition();
						mUserTracker.convertJointCoordinatesToDepth( rPos.x, rPos.y, rPos.z, 
							&(aPoint[s].x), &(aPoint[s].y) );
						Joint[s].x = rPos.x;
						Joint[s].y = rPos.y;
						Joint[s].z = rPos.z;
					}

					cv::putText( cImageBGR, TextLeftHand, cv::Point( aPoint[6].x, aPoint[6].y-50 ), cv::FONT_HERSHEY_SIMPLEX,1.3, cv::Scalar( 165, 40, 40 ),5 );
					cv::putText( cImageBGR, TextRightHand, cv::Point( aPoint[7].x, aPoint[7].y-50 ), cv::FONT_HERSHEY_SIMPLEX,1.3, cv::Scalar( 165, 40, 40 ),5 );
					cv::putText( cImageBGR, TextSettingInfo, cv::Point( aPoint[7].x, aPoint[7].y+50 ), cv::FONT_HERSHEY_SIMPLEX,1, cv::Scalar( 40, 40, 165 ),4 );
					cv::circle( cImageBGR, aPoint[6], 8, cv::Scalar( 0, 0, 255 ), 2 );
					cv::circle( cImageBGR, aPoint[7], 8, cv::Scalar( 0, 0, 255 ), 2 );
					cv::line(cImageBGR,cv::Point( aPoint[8].x-600, aPoint[8].y ),cv::Point( aPoint[8].x+200, aPoint[8].y ),cv::Scalar( 180, 20,85 ),2);
					cv::line(cImageBGR,cv::Point( aPoint[8].x-600, aPoint[8].y ),cv::Point( aPoint[8].x+200, aPoint[8].y ),cv::Scalar( 180, 20,85 ),2);
					cv::line(cImageBGR,cv::Point( aPoint[8].x-600, aPoint[8].y ),cv::Point( aPoint[8].x+200, aPoint[8].y ),cv::Scalar( 180, 20,85 ),2);
					cv::line(cImageBGR,cv::Point( aPoint[8].x-600, aPoint[8].y ),cv::Point( aPoint[8].x+200, aPoint[8].y ),cv::Scalar( 180, 20,85 ),2);
					for( int  s = 0; s < 11; ++ s )
					{
						if( aJoints[s].getPositionConfidence() < 0.5 ){
							cv::circle( cImageBGR, aPoint[s], 8, cv::Scalar( 0 , 255, 255 ), 2 );
							cv::putText( cImageBGR, "interfered!", cv::Point(400, 450 ), cv::FONT_HERSHEY_SIMPLEX,1.3, cv::Scalar( 0, 0, 255 ), 5 );
						}
					}
				}
			}
		}
		cv::imshow( "2013Openni2_DEMO", cImageBGR );
		cv::resizeWindow("2013Openni2_DEMO",ScreenResolutionX,ScreenResolutionY);
		if(AngleMove!=0){
			mDevice.getProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, &value );
			switch(AngleMove) { 
			case 1: 
				mDevice.setProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, value + 1 );
				break; 
			case 2: 
				mDevice.setProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, value - 1 );
				break; 
			default: 
				;
			} 		
			AngleMove = 0;
		}
		if(GetAsyncKeyState(VK_ESCAPE)!=0) break;
		if(GetAsyncKeyState(VK_ADD)!=0) {
			mDevice.getProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, &value );
			mDevice.setProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, value + 1 );
		}
		if(GetAsyncKeyState(VK_SUBTRACT)!=0){
			mDevice.getProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, &value );
			mDevice.setProperty( KINECT_DEVICE_PROPERTY_CAMERA_ELEVATION, value - 1 );
		}
		cv::waitKey( 1 );
	}
	delete midiout;
	mUserTracker.destroy();
	mColorStream.destroy();
	mDepthStream.destroy();
	mDevice.close();
	NiTE::shutdown();
	OpenNI::shutdown();
	return 0;
}






