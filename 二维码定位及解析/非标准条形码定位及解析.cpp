#include "core/core.hpp"
#include "iostream"
#include "highgui/highgui.hpp"
#include "imgproc/imgproc.hpp"
#include "zbar.h"      
#include "cv.h"      
#include "highgui.h"      
#include <iostream>      

using namespace std;
using namespace zbar;  //添加zbar名称空间    
using namespace cv;

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	Rect rect;
	Mat image, imageGray, imageGuussian;
	Mat imageSobelX, imageSobelY, imageSobelOut;
	image = imread("C:\\Users\\Lucky-Chi\\Desktop\\多媒体二维码\\txm.jpg");

	//1. 原图像大小调整，提高运算效率
	resize(image, image, Size(500, 300));
	imshow("1.原图像", image);

	//2. 转化为灰度图
	cvtColor(image, imageGray, CV_RGB2GRAY);
	imshow("2.灰度图", imageGray);

	//3. 高斯平滑滤波
	GaussianBlur(imageGray, imageGuussian, Size(3, 3), 0);
	imshow("3.高斯平衡滤波", imageGuussian);

	//4.求得水平和垂直方向灰度图像的梯度差,使用Sobel算子
	Mat imageX16S, imageY16S;
	Sobel(imageGuussian, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(imageGuussian, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(imageX16S, imageSobelX, 1, 0);
	convertScaleAbs(imageY16S, imageSobelY, 1, 0);
	imageSobelOut = imageSobelX - imageSobelY;
	imshow("4.X方向梯度", imageSobelX);
	imshow("4.Y方向梯度", imageSobelY);
	imshow("4.XY方向梯度差", imageSobelOut);

	//5.均值滤波，消除高频噪声
	blur(imageSobelOut, imageSobelOut, Size(3, 3));
	imshow("5.均值滤波", imageSobelOut);

	//6.二值化
	Mat imageSobleOutThreshold;
	threshold(imageSobelOut, imageSobleOutThreshold, 180, 255, CV_THRESH_BINARY);
	imshow("6.二值化", imageSobleOutThreshold);

	//7.闭运算，填充条形码间隙
	Mat  element = getStructuringElement(0, Size(7, 7));
	morphologyEx(imageSobleOutThreshold, imageSobleOutThreshold, MORPH_CLOSE, element);
	imshow("7.闭运算", imageSobleOutThreshold);

	//8. 腐蚀，去除孤立的点
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	imshow("8.腐蚀", imageSobleOutThreshold);

	//9. 膨胀，填充条形码间空隙
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	imshow("9.膨胀", imageSobleOutThreshold);
	vector<vector<Point>> contours;
	vector<Vec4i> hiera;

	//10.通过findContours找到条形码区域的矩形边界
	findContours(imageSobleOutThreshold, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i<contours.size(); i++)
	{
		rect = boundingRect((Mat)contours[i]);
		rectangle(image, rect, Scalar(255), 2);
	}
	rect = rect + Size(25, 25);
	imshow("10.找出二维码矩形区域", image);
	//	Rect rect2(142, 117, 258, 82);
	Mat img = imread("C:\\Users\\Lucky-Chi\\Desktop\\多媒体二维码\\txm.jpg");
	Mat ROI = img(rect);
	imshow("截取后", ROI);
	imwrite("C:\\Users\\Lucky-Chi\\Desktop\\多媒体二维码\\txm00.jpg", ROI);
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	Mat image2 = imread("C:\\Users\\Lucky-Chi\\Desktop\\多媒体二维码\\txm00.jpg");
	cvtColor(image2, imageGray, CV_RGB2GRAY);
	int width = imageGray.cols;
	int height = imageGray.rows;
	uchar *raw = (uchar *)imageGray.data;
	Image imageZbar(width, height, "Y800", raw, width * height);
	scanner.scan(imageZbar); //扫描条码    
	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		cout << "查询条码失败，请检查图片！" << endl;
	}
	for (; symbol != imageZbar.symbol_end(); ++symbol)
	{
		cout << "类型：" << endl << symbol->get_type_name() << endl << endl;
		cout << "条码：" << endl << symbol->get_data() << endl << endl;
	}
	imshow("Source Image", image);
	waitKey();
	imageZbar.set_data(NULL, 0);
	return 0;
	waitKey();
}