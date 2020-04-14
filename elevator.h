#ifndef Elevator_H

#define Elevator_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief 一些BOOL值以及常量值
 */
/*******************************************************************************************/
#define t 1
#define True -1
#define False 0
#define OK 1
#define Error 0
#define Infeasible -1
#define Overflow -2
#define MaxNum 10
typedef int Status;
#define MaxTime 2000
/*******************************************************************************************/

/**
 * @brief 每个动作消耗的时间
 */
/*******************************************************************************************/
#define testTIM 40           // 测试间隔
#define doorTIM 20           // 开关门所需时间
#define PeopleInAndOutTIM 25 // 乘客进出
#define upstairsTIM 51       // 电梯上楼
#define waitTIM 300          // 最长等待时间
#define downstairsTIM 61     // 电梯下楼
#define accelerateTIM 15     // 加速时间
#define slowdownTIM 14       // 减速时间
/*******************************************************************************************/

/**
 * @brief 一些结构体声明
 */
/*******************************************************************************************/
/* 电梯的运动形式 */
typedef enum ElevatorMove
{
    Opening,      // 正在开门
    Opened,       // 已经开门
    Closing,      // 正在关门
    Closed,       // 已经关门
    Moving,       // 正在移动
    Waiting,      // 等待乘客
    Accelerating, // 正在加速
    SlowingDown,  // 正在减速
} ElevatorMove;

/* 电梯的运动状态 */
typedef enum ElevatorState
{
    GoingUp,   // 上升
    GoingDown, // 下降
    Idle,      // 空闲
} ElevatorState;

/* 乘客的节点信息 */
typedef struct ClientNode
{
    int occurTime;  // 新乘客到达时间
    int ClientID;   // 乘客ID
    int Infloor;    // 起始楼层
    int OutFloor;   // 目标楼层
    int GiveupTime; // 最长等待时间
} Client;

/* 乘客栈 */
typedef struct ClientStackNode
{
    Client base[MaxNum]; // 乘客栈的基地
    int top;             // 栈顶指针
} ClientStack;

/* 等待队列 */
typedef struct ClientQueueNode
{
    Client passenger;             // 乘客
    struct ClientQueueNode *next; // 下一个乘客
} ClientQueueNode;

typedef struct WaitQueue
{
    ClientQueueNode *front;
    ClientQueueNode *rear;
    int WaitNum;
} WaitQueue;

/* 电梯结构体 */
typedef struct ElevatorNode
{
    int ClientNum;         // 当前乘客数量
    ElevatorState State;   // 电梯当前状态
    int curfloor;          // 当前电梯所处楼层
    ElevatorMove movement; // 当前电梯的运动状态
    int movementTim;       // 当前状态剩余时间
    int CallCar[5];        // 目的地
    ClientStack Stack[5];  // 乘客栈
    int D2;                // 值为0,如果电梯已经在某层停候300t
} Elevator;

/*******************************************************************************************/

/**
 * @brief 一些全局变量 
 */
/*******************************************************************************************/
// 总时间计数器
int Time = 0;
// 记录乘客的ID
int ID;
// 两个乘客的间隔
int InterTime = 0;
// 呼叫电梯向上
int CallUp[5];
// 呼叫电梯向下
int CallDown[5];
// 2*5等待队列
WaitQueue Queue[2][5];
/*******************************************************************************************/

/**
 * @brief 一些函数声明
 */
/*******************************************************************************************/

/**
 * @brief 堆栈的声明
 */
/*********************************************/
// 判断堆栈是否为空
Status StackEmpty(ClientStack S);
// 出栈
Status Pop(ClientStack *S, Client *temp);
// 入栈
Status Push(ClientStack *S, Client temp);
/*********************************************/

/**
 * @brief 队列的声明
 */
/*********************************************/
// 初始化队列
Status InitQueue(WaitQueue *Q);
// 入队
Status Enqueue(WaitQueue *Q, Client temp);
// 出队
Status Dequeue(WaitQueue *Q, Client *temp);
// 更新CallUp和CallDown数组
void Update(void);
// 初始化等待队列
Status InitWaitQueue();
/*********************************************/

/**
 * @brief 乘客的声明
 */
/*********************************************/
// 产生新乘客
Status Newclient();
// 删除超过最长等待时间的乘客
Status ClientGiveUp();
// 乘客出电梯栈
Status ClientOut(Elevator *elevator);
// 乘客进电梯栈
Status ClientIn(Elevator *elevator);
// 判断当前楼层有没乘客等待
Status IfPeopleIn(Elevator *elevator);
/*********************************************/

/**
 * @brief 电梯的声明
 */
/*********************************************/
// 初始化电梯
Status InitElevator(Elevator *elevator);
// 电梯空闲时的调度器
Status Controller(Elevator *elevator);
// 改变行动
Status ChangeMovement(Elevator *elevator);
// 高层请求
int HigherRequests(Elevator *elevator);
// 低层请求
int LowerRequests(Elevator *elevator);
// 下楼
Status DownStairs(Elevator *elevator);
// 上楼
Status Upstairs(Elevator *elevator);
// 返回本垒层
Status BackToBase(Elevator *elevator);
// 取消D2
Status CancelD2(Elevator *elevator);
// 改变状态
void ChangeState(Elevator *elevator);
/*********************************************/

/*******************************************************************************************/

#endif // !Elevator_H