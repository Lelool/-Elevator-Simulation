#ifndef Elevator_H

#define Elevator_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define t 1

#define True -1
#define False 0
#define OK 1
#define Error 0
#define Infeasible -1
#define Overflow -2

#define MaxNum 10

typedef int Status;

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

/* 40 20 25 51 300 61 15 14 */
#define testTIM 40           // 测试间隔
#define doorTIM 20           // 开关门所需时间
#define PeopleInAndOutTIM 25 // 乘客进出
#define upstairsTIM 51       // 电梯上楼
#define waitTIM 300          // 最长等待时间
#define downstairsTIM 61     // 电梯下楼
#define accelerateTIM 15     // 加速时间
#define slowdownTIM 14       // 减速时间

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
    int D1;                // 值为0,除非人们正在进入和离开电梯
    int D2;                // 值为0,如果电梯已经在某层停候300t
    int D3;                // 值为0,除非电梯门正开着又无人进出电梯
} Elevator;

#define MaxTime 2000

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

Status StackEmpty(ClientStack S);
Status Pop(ClientStack *S, Client *temp);
Status Push(ClientStack *S, Client temp);

Status InitQueue(WaitQueue *Q);
Status Enqueue(WaitQueue *Q, Client temp);
Status Dequeue(WaitQueue *Q, Client *temp);
void Update(void);
Status InitWaitQueue();

Status Newclient();
Status ClientGiveUp();
Status ClientOut(Elevator *elevator);
Status ClientIn(Elevator *elevator);
Status IfPeopleIn(Elevator *elevator);

Status InitElevator(Elevator *elevator);
Status Controller(Elevator *elevator);
Status ChangeMovement(Elevator *elevator);
int HigherRequests(Elevator *elevator);
int LowerRequests(Elevator *elevator);
Status DownStairs(Elevator *elevator);
Status Upstairs(Elevator *elevator);
Status BackToBase(Elevator *elevator);
Status CancelD2(Elevator *elevator);
void ChangeState(Elevator *elevator);

#endif // !Elevator_H