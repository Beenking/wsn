#include "wsnnode.h"

WSNNode::WSNNode(){}

WSNNode::WSNNode(int _x):x(_x){}

WSNNode::WSNNode(int _x,int _y):x(_x),y(_y){}

WSNNode::WSNNode(int _x,int _y,int _id,double _energy=double(100)):x(_x),y(_y),id(_id),energy(_energy){}

WSNNode::WSNNode(int _x,int _y,int _id,double _energy,Scalar _color):
	x(_x),y(_y),id(_id),energy(_energy),color(_color){}

WSNNode::~WSNNode(void){}


Scalar  WSNNode::NodeColor(){
		if(energy>75)
		{color=Scalar(255,0,0);}  //蓝色
		else if(energy>56.25)
		{color=Scalar(0,255,255);}//黄色
		else if(energy>42.1875)
		{color=Scalar(0,128,255);}//橙色
		else if(energy>31.640625)
		{color=Scalar(0,0,255);}  //红色
		else if(energy>23.73046875)
		{color=Scalar(140,140,140);}//灰色
		else
		{color=Scalar(0,0,0);}    //黑色
		return color;
	}

int WSNNode:: layer (){
		if (distance()<303)
		return 0;
		else if(distance()<428)
			return 1;
		else if(distance()<524)
			return 2;
		else if(distance()<608)
			return 3;
		else
			return 4;
	}


int WSNNode::GetNodeID()const
{return id;}

Scalar WSNNode::GetNodeColor()const
{return color;}
