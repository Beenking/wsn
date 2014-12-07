#include <opencv2\opencv.hpp>

#include "wsntopologybasedenergy.h"
#include "wsnnode.h"

using namespace std;
using namespace cv;

void  help(){
	cout<<"    ///////////////////////////////////////////////////////"<<endl
		<<"      程序模拟WSN网络，其中（0,0）点代表汇聚节点"<<endl
		<<"      每次有随机1-10个传感器节点发送信号"<<endl
		<<"      节点的颜色表示节点的剩余能量ResEnergy"<<endl
		<<"      其中每两次闪烁时间代表一个一个单位时间T"<<endl
		<<"      蓝色：        100% >= ResEnergy > 75%" <<endl
		<<"      黄色：         75% >= ResEnergy > 56.25%"<<endl
		<<"      橙色：      56.25% >= ResEnergy > 42.1875%"<<endl
		<<"      红色：    42.1875% > =ResEnergy > 31.640625%"<<endl
		<<"      灰色：  31.640625% >= ResEnergy > 23.73046875%"<<endl
		<<"      黑色：23.73046875% > =ResEnergy >  0 "<<endl
		<<"      1代表恒定功率发送给Leader节点"<<endl
		<<"      2代表变功率发送给Leader节点且sink节点迁移"<<endl
		<<"    ////////////////////////////////////////////////////////"<<endl
		<<" 请按对应的数字：";
}

void main()
{
	//帮助文档
	help();
	//默认初始化
	WSNTopologyBasedEnergy wsntoplogy;
	//用户选择初始化类型
	char c = getchar();
	switch (c)
	{
	case '1':
		wsntoplogy = WSNTopologyBasedEnergy(WSNTopologyBasedEnergy::ConstPowertoLeader);
		break;
	case '2':
		wsntoplogy = WSNTopologyBasedEnergy(WSNTopologyBasedEnergy::VarPowertoLeaderAndSinkShift);
		break;
	default:
		cout<<"     将以恒定功率发送给Leader节点..."<<endl;
		break;
	}
	cout<<"     按空格键继续..."<<endl;
	//初始化节点
	wsntoplogy.InitNode(600,600,100);
	//每个节点分类
	wsntoplogy.ClassifyNode();
	//画出层数
	wsntoplogy.DrawWSNLayer();
	//找出每层的第一个Linker节点
	wsntoplogy.FindLayerFirstLinkNode(); 
	//连接每一层的所有节点	
	wsntoplogy.LinkNodeEachLayer();
	//网络的生存时间
	wsntoplogy.WSNLifeTime();

	waitKey();
} 
