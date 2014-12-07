#include <ctime>
#include <vector>

#include <opencv2\opencv.hpp>
#include "wsnnode.h"

using namespace std;
using namespace cv;

class WSNTopologyBasedEnergy
{
public:
	enum TransportType
	{
		ConstPowertoLeader = 0,
		VarPowertoLeaderAndSinkShift = 1,
	};

public:
	//构造函数
	WSNTopologyBasedEnergy(TransportType type = ConstPowertoLeader);
	//随机生成点
	int InitNode(int width, int heigth,  int NodeNum);
	//找到每个点的所在层
	void ClassifyNode();
	//画出每一层(以面积）
	void DrawWSNLayer();
	//找到左边第一个连接点
	void FindLayerFirstLinkNode();
	//每一层链接成链
	void LinkNodeEachLayer();
	//WSN网络的终止时间
	int WSNLifeTime();

private:
	//标记生成的点
	void drawCrossNode(Mat &img, WSNNode node, Scalar color);
	//两点间的距离
	double DistanceBetweenNodes(WSNNode node1,WSNNode node2);
	//两点间的距离的平方
	double SDistanceBetweenNodes(WSNNode node1,WSNNode node2);
	//随机选择1-10个结点传送数据。
	vector<int> SelectServalNode();
	//选择leader节点
	void SelectLeaderID();
	//0层为顶层直接与sink节点发送信号
	void TopLayer0SentSink();
	//1层为顶层直接与sink节点发送信号
	void TopLayer1SentSink();
	//2层为顶层直接与sink节点发送信号
	void TopLayer2SentSink();
	//3层为顶层直接与sink节点发送信号
	void TopLayer3SentSink();
	//4层为顶层直接与sink节点发送信号
	void TopLayer4SentSink();

	int MinMark();
	//显示随机选择的1-10个点并发送信号显示能量变化。以变功率发送信号
	void NodeSentSignalVarPower();
	//以恒定功率发送信号
	void NodeSentSignalSamePower();

private:
	//模拟图像
	Mat img;
	//测试
	bool Test;
	//发送类型
	char type_;
	//汇聚节点
	WSNNode sink;
	//能量传送系数
	double alpha;
	//直接与sink节点通信的层号
	int TopLayer;
	//节点的个数
	static const int NODENUM = 100;
	//层数
	static const int LayerNum = 5;
	//节点能量初始值
	double ENERGY;
	//整个网络的能力阈值
	double NetThresEnergy;
	//标记
	int Mark[LayerNum];
	//每层的能量阈值
	double ThresEnergy[LayerNum];
	//初始化的节点
	WSNNode node[NODENUM];
	//每层的第一个链接节点
	WSNNode LayerFirstLinkNode[LayerNum];
	//每层的容器数组
	vector<WSNNode> layer[LayerNum]; 
	//每层的链接顺序容器
	vector<WSNNode> LayerLeaderSeq[LayerNum];
	//每层的Leader节点
	WSNNode LayerLeaderNode[LayerNum];
};
