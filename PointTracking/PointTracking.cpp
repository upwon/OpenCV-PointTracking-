// PointTracking.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include "pch.h"
#include <iostream>
#include<opencv2/video/tracking.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<ctype.h>

using namespace std;
using namespace cv;
//---------全局变量声明---------
Point2f point;
bool addRemovePt = false;



//----------函数声明------------
static void help();
static void onMouse(int event, int x, int y, int /*flags*/, void* /*param*/);






int main()
{
	help();

	VideoCapture cap;

	TermCriteria termcrit(TermCriteria::MAX_ITER | TermCriteria::EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31,31);

	const int MAX_COUNT = 500;
	bool needToInit = false;
	bool nightMode = false;       //夜间模式，默认关闭

	cap.open(0);		//打开摄像头
	if (!cap.isOpened())	//打开失败就报错
	{
		cout << "打开摄像头出错！\n";
		return 0;
	}

	namedWindow("lk 点追踪", 1);
	setMouseCallback("lk 点追踪", onMouse, 0);

	Mat gray, prevgray, image;
	vector<Point2f> points[2];

	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())		//判断是否为空
		{
			break;
		}

		frame.copyTo(image);
		cvtColor(image, gray, COLOR_BGR2GRAY);
		if (nightMode)
			image = Scalar::all(0);
		if (needToInit)
		{
			//初始化
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 3, 0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = false;


		}
		else if (!points[0].empty())
		{
			vector<uchar>status;
			vector<float>err;
			if (prevgray.empty())
			{
				gray.copyTo(prevgray);
			}
			calcOpticalFlowPyrLK(prevgray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if (addRemovePt)
				{
					if (norm(point - points[1][i]) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}
				if (!status[i])
				{
					continue;
				}
				points[1][k++] = points[1][i];
				circle(image, points[1][i], 3, Scalar(0, 255, 0), -1, 8);



			}
			points[1].resize(k);
		}

		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{
			vector<Point2f> tmp;
			tmp.push_back(point);

			cornerSubPix(gray, tmp, winSize, Size(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		imshow("lk 点追踪", image);

		char c = (char)waitKey(10);
		if (27 == c)
		{
			break;

		}
		switch (c)		
		{

		case 'r':
			needToInit = true;
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			break;
		case 'n':
			nightMode = !nightMode;
			break;
		default:
			break;
		}
		swap(points[1], points[0]);
		swap(prevgray, gray);
	}
	





	return 0;

    
}

static void help()
{
	// print a welcome message, and the OpenCV version
	cout << "\n基于Lukas-Kanade 的光流点追踪\n"
		"当前使用的OpenCV版本为： " << CV_VERSION << endl;
	cout << "\n\t程序默认从摄像头读入视频，可以按需改为从视频文件读入图像\n";
	cout << "\n\t操作说明: \n"
		"\t\t通过点击在图像中添加/删除特征点\n"
		"\t\tESC - 退出程序\n"
		"\t\tr -自动进行追踪\n"
		"\t\tc - 删除所有点\n"
		"\t\tn - 开/光-夜晚模式\n" << endl;
	
}

static void onMouse(int event, int x, int y, int /*flags*/, void* /*param*/)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		point = Point2f((float)x, (float)y);
		addRemovePt = true;
	}
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
