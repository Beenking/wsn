#include "wsntopologybasedenergy.h"

WSNTopologyBasedEnergy::WSNTopologyBasedEnergy(TransportType type){
	type_ = type;
	img = Mat(600,600,CV_8UC3,Scalar(255,255,255));
	//是否测试
	Test= true;
	//汇聚节点
	sink=WSNNode(0,0);
	//能量传送系数
	alpha = 0.000014;
	//直接与sink节点通信的层号
	TopLayer=0;
	//节点能量初始值
	ENERGY=100;
	//整个网络的能力阈值
	NetThresEnergy=75;
	for(int i=0;i<LayerNum;i++){
		//标记
		Mark[i]=0;
		//每层的能量阈值
		ThresEnergy[i]=75;
	}
}

//随机生成点
int WSNTopologyBasedEnergy::InitNode(int width, int heigth,  int NodeNum)
{
	if(NodeNum > width * heigth)
		return 0;

	//img = Mat::zeros(width, heigth, CV_8UC3);

	srand((int)time(0));
	for (int i= 0; i< NodeNum; i++) 
	{ 
		int row = rand() % width;
		int col = rand() % heigth;

		node[i]=WSNNode(col,row,i,ENERGY,Scalar(255,0,0));

		//blue色十字标记节点
		drawCrossNode(img,node[i],Scalar(255,0,0));

		//cout<<node[i].layer()<<endl;
		imshow("WSN",img);
		if(true==Test) waitKey(30);
	}
	imshow("WSN", img);
	if(true==Test) waitKey();
	return 1;
}

//找到每个点的所在层
void WSNTopologyBasedEnergy::ClassifyNode()
{
	for(int i=0;i<NODENUM;i++)
	{
		if(0==node[i].layer())
			layer[0].push_back(node[i]);
		else if(1==node[i].layer())
			layer[1].push_back(node[i]);
		else if(2==node[i].layer())
			layer[2].push_back(node[i]);
		else if(3==node[i].layer())
			layer[3].push_back(node[i]);
		else if(4==node[i].layer())
			layer[4].push_back(node[i]);
	}
}

//画出每一层(以面积）
void WSNTopologyBasedEnergy::DrawWSNLayer()
{
	circle(img,Point(0,0),1,Scalar(255,0,255));
	imshow("WSN",img);
	if(true==Test) waitKey(200);

	circle(img,Point(0,0),303,Scalar(255,0,255));
	imshow("WSN",img);
	if(true==Test) waitKey(200);

	circle(img,Point(0,0),428,Scalar(255,0,255));
	imshow("WSN",img);
	if(true==Test) waitKey(200);

	circle(img,Point(0,0),524,Scalar(255,0,255));
	imshow("WSN",img);
	if(true==Test) waitKey(200);

	circle(img,Point(0,0),608,Scalar(255,0,255));
	imshow("WSN",img);
	if(true==Test) waitKey();
}

//找到左边第一个连接点
void WSNTopologyBasedEnergy::FindLayerFirstLinkNode()
{
	vector<WSNNode>::iterator it;

	for(int i=0;i<LayerNum;i++)
	{
		WSNNode FirstLinkNode(600);
		for(it=layer[i].begin();it!=layer[i].end();it++)
		{
			if(it->x<FirstLinkNode.x)
				FirstLinkNode=*it;
		}
		circle(img,Point(FirstLinkNode.x,FirstLinkNode.y),4,Scalar(0,255,0),2,8,0);
		imshow("WSN",img);
		LayerFirstLinkNode[i]=FirstLinkNode;
		if(true==Test) waitKey(200);
	}
	if(true==Test) waitKey();
}

//每一层链接成链
void WSNTopologyBasedEnergy::LinkNodeEachLayer()
{
	vector<WSNNode> temp1;

	for(int i=0;i<LayerNum;i++)
	{
		temp1=layer[i];
		int temp1cap=temp1.size();
		LayerLeaderSeq[i].clear();
		LayerLeaderSeq[i].push_back(LayerFirstLinkNode[i]);
		vector<WSNNode>::iterator it;


		//生成距离数组
		while( LayerLeaderSeq[i].size()<unsigned int(temp1cap+1))

		{
			WSNNode NextNode=*(temp1.begin());
			vector<WSNNode>::iterator p;

			for(it=temp1.begin();it!=temp1.end();it++)
			{
				if( DistanceBetweenNodes(*(LayerLeaderSeq[i].end()-1) ,*it) <= 
					DistanceBetweenNodes(*(LayerLeaderSeq[i].end()-1),NextNode)  )
				{
					NextNode=*it;	
					p=it;
				}
			}

			LayerLeaderSeq[i].push_back(NextNode);
			temp1.erase(p);
		}


		//成链
		LayerLeaderSeq[i].erase(LayerLeaderSeq[i].begin());

		for(it=LayerLeaderSeq[i].begin();it!=LayerLeaderSeq[i].end()-1;it++)
		{
			line(img, Point(it->x,it->y), Point((it+1)->x,(it+1)->y), Scalar(155,155,155),1,8,0);  
			imshow("WSN",img);
			if(true==Test) waitKey(30);
		}
		if(true==Test) waitKey(200);
	}
	if(true==Test) waitKey();
}

//WSN网络的终止时间
int WSNTopologyBasedEnergy::WSNLifeTime()
{
	int i=-1;
	int SentCount=0;
	bool   BREAK=false;
	while(SentCount+1)
	{

		if(type_==ConstPowertoLeader)
			NodeSentSignalSamePower();
		else if(type_==VarPowertoLeaderAndSinkShift)
			NodeSentSignalVarPower();



		for( i=0;i<NODENUM;i++)
		{
			if(node[i].energy<=0)
			{
				BREAK=true;
				//黑圈标记死亡节点
				circle(img,Point(node[i].x,node[i].y),5,Scalar(0,0,0),3,8,0);
				imshow("WSN",img);
				break;
			}
		}

		if(BREAK)
			break;	

		SentCount++;
	}
	cout<<"一个发送了"<<SentCount<<"次"<<endl;

	return i;
}

// 以下是私有成员函数（private function)
//标记生成的点
void WSNTopologyBasedEnergy::drawCrossNode(Mat &img, WSNNode node, Scalar color)
{
	int col = node.x > 2 ? node.x : 2;
	int row = node.y> 2 ? node.y : 2;

	line(img, Point(col -2, row - 2), Point(col + 2, row + 2), color,2,8,0);  
	line(img, Point(col + 2, row - 2), Point(col - 2, row + 2), color,2,8,0);  
}

//两点间的距离
double WSNTopologyBasedEnergy::DistanceBetweenNodes(WSNNode node1,WSNNode node2)
{
	return sqrt(   (node1.x-node2.x)*(node1.x-node2.x) + (node1.y-node2.y)*(node1.y-node2.y)   );
}

//两点间的距离的平方
double WSNTopologyBasedEnergy::SDistanceBetweenNodes(WSNNode node1,WSNNode node2)
{
	return ( (node1.x-node2.x)*(node1.x-node2.x) + (node1.y-node2.y)*(node1.y-node2.y) )  ;
}

//随机选择1-10个结点传送数据。
vector<int> WSNTopologyBasedEnergy::SelectServalNode()
{
	vector<int> SelectNodeSeq;
	vector<int>::iterator it;

	srand((int)time(0));
	int SelectNodeNum=rand()%9+1;
	for(int i=0;i<SelectNodeNum;i++)
	{
		SelectNodeSeq.push_back(rand()%99);
	}

	//闪烁表示发送信号
	for(int i=0;i<LayerNum;i++)
	{
		for(it=SelectNodeSeq.begin();it!=SelectNodeSeq.end();it++)
		{
			drawCrossNode(img,node[*it],Scalar(255,255,255));
		}
		cv::imshow("WSN",img);
		if(true==Test) waitKey(70);
		for(it=SelectNodeSeq.begin();it!=SelectNodeSeq.end();it++)
		{
			drawCrossNode(img,node[*it],node[*it].NodeColor());	
		}
		cv::imshow("WSN",img);
		if(true==Test) waitKey(70);
	}
	return SelectNodeSeq;
}

//选择leader节点
void WSNTopologyBasedEnergy::SelectLeaderID()
{
	vector<WSNNode>::iterator it;
	int i=0;
	while(i<LayerNum)
	{
		int j=0;
		bool   BREAK=false;
		for(it=LayerLeaderSeq[i].begin();it!=LayerLeaderSeq[i].end();it++)
		{
			if(node[it->GetNodeID()].energy>=ThresEnergy[i])
			{
				LayerLeaderNode[i]=*it;
				drawCrossNode(img,node[LayerLeaderNode[i].GetNodeID()],node[LayerLeaderNode[i].GetNodeID()].NodeColor());
				imshow("WSN",img);
				i++;
				BREAK=true;
				break;
			}
			j++;
		}
		if(BREAK) continue;
		if(j==LayerLeaderSeq[i].size())
		{
			ThresEnergy[i]=3*ThresEnergy[i]/4;
			Mark[i]++;
		}

	}
}

//0层为顶层直接与sink节点发送信号
void WSNTopologyBasedEnergy::TopLayer0SentSink()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		Mat tempimg=img.clone();
		//选择每层的leader节点
		SelectLeaderID();

		// 链接所有leader节点
		line(tempimg,Point(0,0),Point(LayerLeaderNode[0].x,LayerLeaderNode[0].y),Scalar(45,150,45),1,8,0);//用黑绿色直线连接sink节点和顶层节点
		for(int i=0;i<LayerNum-1;i++)
		{
			line(tempimg,Point(LayerLeaderNode[i].x,LayerLeaderNode[i].y),Point(LayerLeaderNode[i+1].x,LayerLeaderNode[i+1].y),Scalar(0,255,0));
			imshow("WSN",tempimg);
		}
		if(true==Test) waitKey(100);


		//计算能量变化值
		if(0==node[*it].layer())
		{
			node[LayerLeaderNode[0].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[0],sink);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[0]);
		}
		else if(1==node[*it].layer())
		{
			node[LayerLeaderNode[1].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[1],LayerLeaderNode[0]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[1]);
			node[LayerLeaderNode[0].GetNodeID()].energy-=1;
		}
		else if(2==node[*it].layer())
		{
			node[LayerLeaderNode[2].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[2],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[2]);
			node[LayerLeaderNode[0].GetNodeID()].energy-=1;
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;

		}

		else if(3==node[*it].layer())
		{
			node[LayerLeaderNode[3].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[3],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[3]);
			node[LayerLeaderNode[0].GetNodeID()].energy-=1;
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
		}
		else 
		{
			node[LayerLeaderNode[4].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[4],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[4]);
			node[LayerLeaderNode[0].GetNodeID()].energy-=1;
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<NODENUM;i++)
	{
		drawCrossNode(img,node[i],node[i].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}

//1层为顶层直接与sink节点发送信号
void WSNTopologyBasedEnergy::TopLayer1SentSink()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		Mat tempimg=img.clone();

		//选择每层的leader节点
		SelectLeaderID();


		// 链接所有leader节点
		line(tempimg,Point(0,0),Point(LayerLeaderNode[1].x,LayerLeaderNode[1].y),Scalar(45,150,45),1,8,0);//用黑绿色直线连接sink节点和顶层节点
		for(int i=0;i<LayerNum-1;i++)
		{
			line(tempimg,Point(LayerLeaderNode[i].x,LayerLeaderNode[i].y),Point(LayerLeaderNode[i+1].x,LayerLeaderNode[i+1].y),Scalar(0,255,0));
			imshow("WSN",tempimg);
		}
		if(true==Test) waitKey(100);


		//计算能量变化值
		if(0==node[*it].layer())
		{
			node[LayerLeaderNode[0].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[0],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[0]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
		}
		else if(1==node[*it].layer())
		{
			node[LayerLeaderNode[1].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[1],sink);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[1]);
		}
		else if(2==node[*it].layer())
		{
			node[LayerLeaderNode[2].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[2],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[2]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;

		}

		else if(3==node[*it].layer())
		{
			node[LayerLeaderNode[3].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[3],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[3]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
		}
		else 
		{
			node[LayerLeaderNode[4].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[4],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[4]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<NODENUM;i++)
	{
		drawCrossNode(img,node[i],node[i].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}

//2层为顶层直接与sink节点发送信号
void WSNTopologyBasedEnergy::TopLayer2SentSink()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		Mat tempimg=img.clone();

		//选择每层的leader节点
		SelectLeaderID();

		// 链接所有leader节点
		line(tempimg,Point(0,0),Point(LayerLeaderNode[2].x,LayerLeaderNode[2].y),Scalar(45,150,45),1,8,0);//用黑绿色直线连接sink节点和顶层节点
		for(int i=0;i<LayerNum-1;i++)
		{
			line(tempimg,Point(LayerLeaderNode[i].x,LayerLeaderNode[i].y),Point(LayerLeaderNode[i+1].x,LayerLeaderNode[i+1].y),Scalar(0,255,0));
			imshow("WSN",tempimg);
		}
		if(true==Test) waitKey(100);


		//计算能量变化值
		if(0==node[*it].layer())
		{
			node[LayerLeaderNode[0].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[0],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[0]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
		}
		else if(1==node[*it].layer())
		{
			node[LayerLeaderNode[1].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[1],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[1]);
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
		}
		else if(2==node[*it].layer())
		{
			node[LayerLeaderNode[2].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[2],sink);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[2]);
		}

		else if(3==node[*it].layer())
		{
			node[LayerLeaderNode[3].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[3],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[3]);
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
		}
		else 
		{
			node[LayerLeaderNode[4].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[4],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[4]);
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<NODENUM;i++)
	{
		drawCrossNode(img,node[i],node[i].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}

//3层为顶层直接与sink节点发送信号
void WSNTopologyBasedEnergy::TopLayer3SentSink()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		Mat tempimg=img.clone();

		//选择每层的leader节点
		SelectLeaderID();


		// 链接所有leader节点
		line(tempimg,Point(0,0),Point(LayerLeaderNode[3].x,LayerLeaderNode[3].y),Scalar(45,150,45),1,8,0);//用黑绿色直线连接sink节点和顶层节点
		for(int i=0;i<LayerNum-1;i++)
		{
			line(tempimg,Point(LayerLeaderNode[i].x,LayerLeaderNode[i].y),Point(LayerLeaderNode[i+1].x,LayerLeaderNode[i+1].y),Scalar(0,255,0));
			imshow("WSN",tempimg);
		}
		if(true==Test) waitKey(100);


		//计算能量变化值
		if(0==node[*it].layer())
		{
			node[LayerLeaderNode[0].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[0],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[0]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}
		else if(1==node[*it].layer())
		{
			node[LayerLeaderNode[1].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[1],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[1]);
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}
		else if(2==node[*it].layer())
		{
			node[LayerLeaderNode[2].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[2],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[2]);
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}

		else if(3==node[*it].layer())
		{
			node[LayerLeaderNode[3].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[3],sink);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[3]);
		}
		else 
		{
			node[LayerLeaderNode[4].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[4],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[4]);
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<NODENUM;i++)
	{
		drawCrossNode(img,node[i],node[i].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}

//4层为顶层直接与sink节点发送信号
void WSNTopologyBasedEnergy::TopLayer4SentSink()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		Mat tempimg=img.clone();

		//选择每层的leader节点
		SelectLeaderID();


		// 链接所有leader节点
		line(tempimg,Point(0,0),Point(LayerLeaderNode[4].x,LayerLeaderNode[4].y),Scalar(45,150,45),1,8,0);//用黑绿色直线连接sink节点和顶层节点
		for(int i=0;i<LayerNum-1;i++)
		{
			line(tempimg,Point(LayerLeaderNode[i].x,LayerLeaderNode[i].y),Point(LayerLeaderNode[i+1].x,LayerLeaderNode[i+1].y),Scalar(0,255,0));
			imshow("WSN",tempimg);
		}
		if(true==Test) waitKey(100);


		//计算能量变化值
		if(0==node[*it].layer())
		{
			node[LayerLeaderNode[0].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[0],LayerLeaderNode[1]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[0]);
			node[LayerLeaderNode[1].GetNodeID()].energy-=1;
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
			node[LayerLeaderNode[4].GetNodeID()].energy-=1;
		}
		else if(1==node[*it].layer())
		{
			node[LayerLeaderNode[1].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[1],LayerLeaderNode[2]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[1]);
			node[LayerLeaderNode[2].GetNodeID()].energy-=1;
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
			node[LayerLeaderNode[4].GetNodeID()].energy-=1;
		}
		else if(2==node[*it].layer())
		{
			node[LayerLeaderNode[2].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[2],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[2]);
			node[LayerLeaderNode[3].GetNodeID()].energy-=1;
			node[LayerLeaderNode[4].GetNodeID()].energy-=1;
		}

		else if(3==node[*it].layer())
		{
			node[LayerLeaderNode[3].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[3],LayerLeaderNode[3]);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[3]);
			node[LayerLeaderNode[4].GetNodeID()].energy-=1;
		}
		else 
		{
			node[LayerLeaderNode[4].GetNodeID()].energy-=1+alpha * SDistanceBetweenNodes(LayerLeaderNode[4],sink);
			node[*it].energy-=0.5+alpha * SDistanceBetweenNodes(node[*it],LayerLeaderNode[4]);
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<NODENUM;i++)
	{
		drawCrossNode(img,node[i],node[i].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}


int WSNTopologyBasedEnergy::MinMark()
{
	int min=Mark[0];
	int j=0;
	for(int i=0;i<LayerNum;i++)
	{
		if(Mark[i]<min)
		{
			min=Mark[i];
			j=i;
		}
	}
	return j;
}
// 1 0 1 0 0

//显示随机选择的1-10个点并发送信号显示能量变化。以变功率发送信号
void WSNTopologyBasedEnergy::NodeSentSignalVarPower()
{
	if(0==MinMark())
		TopLayer0SentSink();
	else if(1==MinMark())
		TopLayer1SentSink();
	else if(2==MinMark())
		TopLayer2SentSink();
	else if(3==MinMark())
		TopLayer3SentSink();
	else 
		TopLayer4SentSink();

	/*if(0==Mark[0])
	TopLayer0SentSink();
	else if(Mark[0]>=1&&0==Mark[1])
	TopLayer1SentSink();
	else if(Mark[0]>=1&&Mark[1]>=1&&0==Mark[2])
	TopLayer2SentSink();
	else if(Mark[0]>=1&&Mark[1]>=1&&Mark[2]>=1&& 0==Mark[3])
	TopLayer3SentSink();
	else 
	TopLayer4SentSink();*/
}

//以恒定功率发送信号
void WSNTopologyBasedEnergy::NodeSentSignalSamePower()
{
	vector<int>::iterator it;
	vector<int> _array=SelectServalNode();
	for(it=_array.begin();it!=_array.end();it++)
	{
		//选择每层的leader节点
		SelectLeaderID();

		//计算能量变化值
		if(0==node[*it].layer())
		{
			//LayerLeaderNode[0].energy-=1+alpha * DistanceBetweenNodes(LayerLeaderNode[0],Link);
			node[LayerLeaderNode[0].GetNodeID()].energy-=10;
			node[*it].energy-=7;
		}
		else if(1==node[*it].layer())
		{
			//LayerLeaderNode[1].energy-=5;
			node[LayerLeaderNode[1].GetNodeID()].energy-=10;
			node[*it].energy-=7;
		}
		else if(2==node[*it].layer())
		{
			//LayerLeaderNode[2].energy-=5;
			node[LayerLeaderNode[2].GetNodeID()].energy-=10;
			node[*it].energy-=7;
		}

		else if(3==node[*it].layer())
		{
			//LayerLeaderNode[3].energy-=5;
			node[LayerLeaderNode[3].GetNodeID()].energy-=10;
			node[*it].energy-=7;
		}
		else 
		{
			//LayerLeaderNode[4].energy-=5;
			node[LayerLeaderNode[4].GetNodeID()].energy-=10;
			node[*it].energy-=7;
		}

		//不同的能量的节点画不同的颜色
		drawCrossNode(img,node[*it],node[*it].NodeColor());
		imshow("WSN",img);	
	}
	//Leader节点的颜色变化
	for(int i=0;i<LayerNum;i++)
	{
		drawCrossNode(img,node[LayerLeaderNode[i].GetNodeID()],node[LayerLeaderNode[i].GetNodeID()].NodeColor());
		imshow("WSN",img);
	}
	if(true==Test) waitKey(400);   //两次发送的时间间隔
}
