#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <wiringPi.h>

//Goruntu Isleme Degiskenleri
using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame, Matrix, framePers, frameGray, frameThresh, frameEdge, frameFinal, frameFinalDuplicate, frameFinalDuplicate1;
Mat ROILane, ROILaneEnd;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result, laneEnd;
RaspiCam_Cv Camera;

stringstream ss;

vector<int> histogramLane;
vector<int> histogramLaneEnd;

Point2f Source[] = {Point2f(34,135),Point2f(366,135),Point2f(5,185),Point2f(395,185)};
Point2f Destination[] = {Point2f(109,0),Point2f(291,0),Point2f(109,240),Point2f(291,240)};

//Makina Ogrenmesi Degiskenleri
CascadeClassifier Stop_Cascade,Object_Cascade;
Mat frame_stop, ROI_stop, gray_stop, frame_Object, ROI_Object ,gray_Object;
vector<Rect> Stop, Object;
int dist_Stop, dist_Object;

 
 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )               //Kamera Ayarlari
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,400) );    
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0));

}

void Perspective()                                                      //Kamera Goruntusunu Perspektife Cevirme 
{
	line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame,Source[2], Source[0], Scalar(0,0,255), 2);
	
	Matrix = getPerspectiveTransform(Source , Destination);
	warpPerspective(frame, framePers, Matrix, Size(400,240));
	
}

void Threshold()                  					
{
    cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray, 100, 255, frameThresh);
    Canny(frameGray, frameEdge, 600,  700, 3, false);
    add(frameThresh, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
    cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);
    cvtColor(frameFinal, frameFinalDuplicate1, COLOR_RGB2BGR);
}


void Histogram()
{
    histogramLane.resize(400);
    histogramLane.clear();
    
    for(int i=0; i<350; i++)
    {
	ROILane = frameFinalDuplicate(Rect(i,140,1,100));
	divide(255, ROILane, ROILane);
	histogramLane.push_back((int)(sum(ROILane)[0]));
	
    }
	
    histogramLaneEnd.resize(400);
    histogramLaneEnd.clear();
    
    for(int i=0; i<350; i++)
    {
	ROILaneEnd = frameFinalDuplicate1(Rect(i,0,1,240));
	divide(255, ROILaneEnd, ROILaneEnd);
	histogramLaneEnd.push_back((int)(sum(ROILaneEnd)[0]));
	
    }
     
     laneEnd = sum(histogramLaneEnd)[0];
     cout<<"Lane End = "<<laneEnd<<endl;
	
    
    
}

void LaneFinder()							//Cizgi Bulma
{
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histogramLane.begin(), histogramLane.begin() +150);
    LeftLanePos = distance(histogramLane.begin(), LeftPtr);
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histogramLane.begin() +250, histogramLane.end());
    RightLanePos = distance(histogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255, 0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0, 255, 0), 2);
}

void LaneCenter()							//Merkez Cizgi
{
    laneCenter = (RightLanePos - LeftLanePos)/ 2 + LeftLanePos;
    frameCenter = 192;
    
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);
    
    Result = laneCenter-frameCenter;
}

void Capture()			
{
    Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    cvtColor(frame, frame_stop, COLOR_BGR2RGB);
    cvtColor(frame, frame_Object, COLOR_BGR2RGB);
}

void Stop_detection()							//DUR Tabelasi Veri Seti ve Tabela Ile Araba Arasindaki Mesafe 
{
    if(!Stop_Cascade.load("//home//pi//Desktop//Otonom Surus//Stop_cascade.xml"))
    {
	printf("Unable to open stop cascade file");
    }
    
    ROI_stop = frame_stop(Rect(200,0,200,180));
    cvtColor(ROI_stop, gray_stop, COLOR_RGB2GRAY);
    equalizeHist(gray_stop, gray_stop);
    Stop_Cascade.detectMultiScale(gray_stop, Stop);
    
    for(int i=0; i<Stop.size(); i++)
    {
	Point P1(Stop[i].x, Stop[i].y);
	Point P2(Stop[i].x + Stop[i].width, Stop[i].y + Stop[i].height);
	
	rectangle(ROI_stop, P1, P2, Scalar(0, 0, 255), 2);
	putText(ROI_stop, "DUR ", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
	dist_Stop = (-1.23)*(P2.x-P1.x) + 102.597;
	
	ss.str(" ");
        ss.clear();
	ss<<"Mesafe="<<dist_Stop<<"cm";
        putText(ROI_stop, ss.str(), Point2f(1,25), 0,0.6, Scalar(0,0,255), 2);
    }
}

void Object_detection()							//Obje Veri seti ve Obje Ile Araba Arasindaki Mesafe
{
    if(!Object_Cascade.load("//home//pi//Desktop//Otonom Surus//Object_cascade.xml"))
    {
	printf("Unable to open Object cascade file");
    }
    
    ROI_Object = frame_Object(Rect(80,20,180,140));
    cvtColor(ROI_Object, gray_Object, COLOR_RGB2GRAY);
    equalizeHist(gray_Object, gray_Object);
    Object_Cascade.detectMultiScale(gray_Object, Object);
    
    for(int i=0; i<Object.size(); i++)
    {
	Point P1(Object[i].x, Object[i].y);
	Point P2(Object[i].x + Object[i].width, Object[i].y + Object[i].height);
	
	rectangle(ROI_Object, P1, P2, Scalar(0, 0, 255), 2);
	putText(ROI_Object, "Object", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
	dist_Object = (-0.48)*(P2.x-P1.x) + 98.6;
	
       ss.str(" ");
       ss.clear();
       ss<<"Mesafe = "<<dist_Object<<"cm";
       putText(ROI_Object, ss.str(), Point2f(1,130), 0,0.6, Scalar(0,0,255), 2);
	
    }
    
}

     



int main(int argc,char **argv)						//Arduino ile Haberlesme Yapan Pinler
{
	wiringPiSetup();
	pinMode(21, OUTPUT);
        pinMode(25, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);
	
	Setup(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open())
	{
		
	cout<<"Failed to Connect"<<endl;
     }
     
     cout<<"Camera Id = "<<Camera.getId()<<endl;
     
     
     
     
    
    while(1)								//Raspberry-Arduino Kontrol Loopu
    {
    auto start = std::chrono::system_clock::now();
    
    Capture();
    Perspective();
    Threshold();
    Histogram();
    LaneFinder();
    LaneCenter();
    Stop_detection();
    Object_detection();
    
    if (dist_Stop > 10 && dist_Stop < 30)
    {
	digitalWrite(21, 0);
	digitalWrite(25, 0);    //decimal = 8
	digitalWrite(23, 0);
	digitalWrite(24, 1);
	cout<<"DUR"<<endl;
	dist_Stop = 0;
    
	goto Stop_Sign;
    }
    
    if (dist_Object > 30 && dist_Object < 45)
    {
	digitalWrite(21, 1);
	digitalWrite(25, 0);    //decimal = 9
	digitalWrite(23, 0);
	digitalWrite(24, 1);
	cout<<"Object"<<endl;
	dist_Object = 0;
    
	goto Object;
    }
    
    if (laneEnd > 5000)
    
    {
	
	digitalWrite(21, 1);
	digitalWrite(25, 1);    //decimal = 7
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Lane End"<<endl;
    }
    
    
    else if (Result == 0)
    {
	digitalWrite(21, 0);
	digitalWrite(25, 0);    //decimal = 0
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Forward"<<endl;
    }
    
        
    else if (Result > 0 && Result < 10)
    {
	digitalWrite(21, 0);
	digitalWrite(25, 0);    //decimal = 4
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left1"<<endl;
    }
    
    else if (Result >= 10 && Result < 20)
    {
	digitalWrite(21, 1);
	digitalWrite(25, 0);    //decimal = 5
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left2"<<endl;
    }
    
    else if (Result >= 20)
    {
	digitalWrite(21, 0);
	digitalWrite(25, 1);    //decimal = 6
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left3"<<endl;
    }
    
    else if (Result < 0 && Result > -10)
    {
	digitalWrite(21, 1);
	digitalWrite(25, 0);    //decimal = 1
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right1"<<endl;
    }
    
    else if (Result <= -10 && Result > -20)
    {
	digitalWrite(21, 0);
	digitalWrite(25, 1);    //decimal = 2
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right2"<<endl;
    }
    
    else if (Result <= -20)
    {
	digitalWrite(21, 1);
	digitalWrite(25, 1);    //decimal = 3
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right3"<<endl;
    }
    
    else if (Result == 999)
    {
	return Result; 
    }
    else
    {
	cout<<"Yoldan Cikti"<<endl;
	
	
    }
    
    Stop_Sign:
    Object:
    
    if (laneEnd > 5000)
    {
    ss.str(" ");
    ss.clear();
    ss<<"Lane End";
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(255,0,0), 2);

    }
   
    
    else if (Result ==0)
    {
    ss.str(" ");
    ss.clear();
    ss<<"Result = "<<Result<<" (Move Forward)";
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);

    }
    
    else if (Result > 0)
    {
    ss.str(" ");
    ss.clear();
    ss<<"Result = "<<Result<<" (Move Left)";
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);

    }
    else if (Result < 0)
    {
    ss.str(" ");
    ss.clear();
    ss<<"Result = "<<Result<<" (Move Right)";
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);

    }
    
    namedWindow("Original", WINDOW_KEEPRATIO);
    moveWindow("Original",0,100);
    resizeWindow("Original",400,240);
    imshow("Original", frame);
    
    
    namedWindow("Perspective", WINDOW_KEEPRATIO);
    moveWindow("Perspective",400,100);
    resizeWindow("Perspective",400,240);
    imshow("Perspective", framePers);
    
    
    namedWindow("Final", WINDOW_KEEPRATIO);
    moveWindow("Final",800,100);
    resizeWindow("Final",400,240);
    imshow("Final", frameFinal);
    
    namedWindow("DUR ", WINDOW_KEEPRATIO);
    moveWindow("DUR ", 800, 360);
    resizeWindow("DUR ", 400, 240);
    imshow("DUR ", ROI_stop);
    
    
    namedWindow("ARAC ", WINDOW_KEEPRATIO);
    moveWindow("ARAC ", 400, 360);
    resizeWindow("ARAC ", 400, 240);
    imshow("ARAC ", ROI_Object);
    
    waitKey(1);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
    cout<<"FPS = "<<FPS<<endl;
    
  
    
    
    
    }

    
    return 0;
     
}

