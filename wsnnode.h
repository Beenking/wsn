#pragma once
#include <math.h>

#include "opencv2/opencv.hpp"

using namespace cv;

class WSNNode
{
public:
	//构造函数
	WSNNode();
	WSNNode(int );
	WSNNode(int ,int );
	WSNNode(int _x,int _y,int _number,double _energy);
	WSNNode(int _x,int _y,int _number,double _energy,Scalar _color);
	//析构函数
	~WSNNode(void);
	//到sink节点的距离
	float distance()const{return sqrt(float(x*x+y*y));}
	//节点所在层
	int layer ();
	//节点的颜色
	Scalar NodeColor();
	//节点的id号
	int GetNodeID()const;
	//节点的颜色
	Scalar GetNodeColor()const;

public:
	//坐标（x,y）
	int x,y;
	//当前剩余能量
	double energy;

private:
	//当前层的Leader节点的ID号
	int leaderID;
	//当前节点的id号
	int id;
	// 节点的颜色
	Scalar color;
};

