#include "elevator.h"

/* Elevator */
/*****************************************************************************************/
Status InitElevator(Elevator *elevator)
{
    // 初始化CallCar以及CallUp和CallDown
    for (int i = 0; i < 5; i++)
    {
        elevator->CallCar[i] = 0;
        CallDown[i] = 0;
        CallUp[i] = 0;
        elevator->Stack[i].top = -1;
    }
    elevator->curfloor = 1;
    elevator->D2 = 0;
    elevator->movement = Waiting;
    elevator->movementTim = waitTIM;
    elevator->State = Idle;
    elevator->ClientNum = 0;
}

Status Controller(Elevator *elevator)
{

    if (elevator->State != Idle)
        return OK;

    int highrequest = HigherRequests(elevator);
    int lowrequest = LowerRequests(elevator);

    // 记录将要去的下一层楼
    int nextfloor = -1;

    // 如果楼上楼下都有请求则选择更近的请求
    if (highrequest != -1 && lowrequest != -1)
    {
        // 如果楼下请求更近,那么nextfloor等于楼下请求
        if (highrequest - elevator->curfloor >= elevator->curfloor - lowrequest)
        {
            nextfloor = lowrequest;
        }
        // 如果楼上请求更近,那么nextfloor等于楼上请求
        else
        {
            nextfloor = highrequest;
        }
    }
    // 如果楼上没请求,楼下有请求
    else if (highrequest == -1)
    {
        // nextfloor等于楼下请求
        nextfloor = lowrequest;
    }
    // 如果楼下没请求,楼上有请求
    else if (lowrequest == -1)
    {
        // nextfloor等于楼上请求
        nextfloor = highrequest;
    }

    // 如果没有请求直接返回
    if (nextfloor == -1)
    {
        return OK;
    }
    // 如果是本层有请求,切换状态为正在开门,并且设置TIM
    else if (nextfloor == elevator->curfloor)
    {
        // 设置开门状态
        elevator->movement = Opening;
        // 设置TIM
        elevator->movementTim = doorTIM;
    }
    // 如果楼下请求
    else if (nextfloor < elevator->curfloor)
    {
        // 设置电梯状态为下降
        elevator->State = GoingDown;
        // 当前行为为正在加速
        elevator->movement = Accelerating;
        // 设置TIM
        elevator->movementTim = accelerateTIM;
    }
    // 如果是楼上请求
    else
    {
        // 设置上升状态
        elevator->State = GoingUp;
        // 加速行为
        elevator->movement = Accelerating;
        // 加速时间
        elevator->movementTim = accelerateTIM;
    }
}

/**
 * @brief 改变电梯运动状态
 */
Status ChangeMovement(Elevator *elevator)
{
    switch (elevator->movement)
    {
    // 开门结束切换到Opened
    case Opening:
        elevator->movement = Opened;
        printf("Opened the door successfully!\n");
        elevator->movementTim = testTIM;
        break;

    // 乘客进出,完成后切换到Closing
    case Opened:

        // 如果电梯栈不空则乘客出电梯
        if (StackEmpty(elevator->Stack[elevator->curfloor]) == False)
        {
            // 乘客出栈
            ClientOut(elevator);
            // 计时
            elevator->movementTim = PeopleInAndOutTIM;
        }
        // 乘客都出栈后进行下一个判断
        else
        {
            // 更新当前楼层CallCar数组
            elevator->CallCar[elevator->curfloor] = 0;
            // 如果当前行进方向上有人等待并且电梯未满
            int retval = IfPeopleIn(elevator);
            if (retval == True && elevator->ClientNum < MaxNum)
            {
                // 乘客入栈
                ClientIn(elevator);
                // 计时
                elevator->movementTim = PeopleInAndOutTIM;
                // 取消D2
                CancelD2(elevator);
            }
            // 如果没人等待那么进入下一个状态
            else
            {
                elevator->movement = Closing;
                printf("Now the elevator is closing the door.\n");
                elevator->movementTim = doorTIM;
            }
        }

        break;

    // 关门结束后切换到Closed
    case Closing:
        // 关门
        elevator->movement = Closed;
        // 打印消息
        printf("Closed the door successfully!\n");
        break;

    // 关门后切换如果有请求则设置为Accelerating否则切换到Waiting
    case Closed:
        // 如果有请求,设置电梯状态为正在加速
        if (HigherRequests(elevator) != -1 || LowerRequests(elevator) != -1)
        {
            elevator->movement = Accelerating;
            // 计时
            elevator->movementTim = accelerateTIM;
            // 打印提示消息
            printf("Elevator is accelerating.\n");
        }
        // 上方的请求局限于与电梯方向相同,如果不相同并且电梯内还有乘客,就改变运动状态
        else if (elevator->ClientNum)
        {
            ChangeState(elevator);
        }
        // 如果没有请求并且没有人那就等待
        else
        {
            elevator->movement = Waiting;
            elevator->movementTim = waitTIM;
            // 提示消息
            printf("Elevator is waiting at %d floor\n", elevator->curfloor);
        }

        break;

    // 判断是否应该继续移动或者减速
    case Moving:
        // D2为1表示电梯正在返回本垒层
        if (elevator->D2)
        {
            // 如果当前楼层在本垒层下方
            if (elevator->curfloor < 1)
            {
                // 上楼
                Upstairs(elevator);
                // 继续移动
                elevator->movement = Moving;
                // 计时
                elevator->movementTim = upstairsTIM;
            }
            // 如果当前楼层在本垒层上方
            else if (elevator->curfloor > 1)
            {
                // 下楼
                DownStairs(elevator);
                // 继续移动
                elevator->movement = Moving;
                // 计时
                elevator->movementTim = downstairsTIM;
            }
            // 如果到达了本垒层
            else
            {
                // 紧急刹车
                elevator->movement = SlowingDown;
                // 计时
                elevator->movementTim = slowdownTIM;
            }
        }
        else
        {
            // 记录nextfloor的值
            int nextfloor;
            // 如果电梯正在上楼就获取楼上的请求
            if (elevator->State == GoingUp)
            {
                nextfloor = HigherRequests(elevator);
            }
            // 如果电梯正在下楼就获取楼下的请求
            else
            {
                nextfloor = LowerRequests(elevator);
            }
            // 如果当前楼层,则电梯紧急刹车
            if (nextfloor == elevator->curfloor)
            {
                elevator->movement = SlowingDown;
                elevator->movementTim = slowdownTIM;
            }
            // 如果应该继续往下那么更新当前状态的时间并下楼
            else if (nextfloor < elevator->curfloor)
            {
                DownStairs(elevator);
                elevator->movement = Moving;
                elevator->movementTim = downstairsTIM;
            }
            // 如果应该继续往上那么更新当前状态的时间并上楼
            else
            {
                Upstairs(elevator);
                elevator->movement = Moving;
                elevator->movementTim = upstairsTIM;
            }
        }

        break;

    // 等待完成后转到本垒层
    case Waiting:
        // 等待超过300s后如果不在本垒层就准备返回本垒层
        // 表明等待时间已经超过300s

        // 如果不在本垒层则返回本垒层,如果在本垒层则保持现状
        BackToBase(elevator);

        break;

    // 加速完成后切换到Moving
    case Accelerating:

        // 加速完成之后电梯正常移动
        elevator->movement = Moving;
        // 如果电梯正在下楼
        if (elevator->State == GoingDown)
        {
            // 计时为下楼计时
            elevator->movementTim = downstairsTIM;
        }
        // 如果电梯正在上楼
        else if (elevator->State == GoingUp)
        {
            // 计时为上楼计时
            elevator->movementTim = upstairsTIM;
        }

        break;

    // 减速完成后切换到Opening
    case SlowingDown:
        // 如果是正常减速,下一步就是开门
        if (!elevator->D2)
        {
            // 减速完成之后开门
            elevator->movement = Opening;
            // 计时
            elevator->movementTim = doorTIM;
        }
        // 如果回到本垒层的减速,则切换为等待
        else
        {
            elevator->movement = Waiting;
            elevator->movementTim = waitTIM;
        }

        break;
    }
}

/**
 * @brief  返回楼上请求
 * @retval 如果有请求则返回最接近当前楼层的请求,否则返回-1
 */
int HigherRequests(Elevator *elevator)
{
    // 电梯状态得为GoingUp或者Wating才能考虑上楼
    if (elevator->State == GoingUp || elevator->movement == Waiting)
    {
        // 找到最接近的请求
        for (int i = elevator->curfloor; i < 5; i++)
        {
            if (elevator->CallCar[i] || CallUp[i] || CallDown[i])
            {
                // 返回最接近的请求楼层
                return i;
            }
        }
    }
    return -1;
}

/**
 * @brief  返回楼下请求
 * @retval 如果有请求则返回最近楼层请求,否则返回-1
 */
int LowerRequests(Elevator *elevator)
{
    // 电梯状态得为GoingDown或者Waiting才行
    if (elevator->State == GoingDown || elevator->movement == Waiting)
    {
        // 寻找最近请求
        for (int i = elevator->curfloor; i >= 0; i--)
        {
            if (elevator->CallCar[i] || CallUp[i] || CallDown[i])
            {
                // 返回最近请楼层
                return i;
            }
        }
    }
    // 没找到则返回-1
    return -1;
}

/**
 * @brief 改变电梯的状态
 */
void ChangeState(Elevator *elevator)
{
    if (elevator->ClientNum >= MaxNum)
        return;
    int temp = 0;
    // 看楼上是否有请求
    for (int i = elevator->curfloor + 1; i < 5; i++)
    {
        if (elevator->CallCar[i] || CallUp[i])
        {
            temp = 1;
        }
    }
    // 如果楼上没有请求并且当前电梯内有乘客,那么电梯State需要改变
    if (elevator->ClientNum && !temp)
    {
        elevator->State = GoingDown;
    }

    temp = 0;
    // 看楼下是否有请求
    for (int i = elevator->curfloor - 1; i >= 0; i--)
    {
        if (elevator->CallCar[i] || CallDown[i])
        {
            temp = 1;
        }
    }
    // 如果楼下没有请求并且当前电梯内有乘客,那么电梯State需要改变
    if (elevator->ClientNum && !temp)
    {
        elevator->State = GoingUp;
    }
}

/**
 * 
 */
Status Upstairs(Elevator *elevator)
{
    elevator->curfloor++;
}

/**
 * 
 */
Status DownStairs(Elevator *elevator)
{
    elevator->curfloor--;
}

/**
 * @brief 返回到本垒层
 */
Status BackToBase(Elevator *elevator)
{
    // 表示等待时间已经超过300s
    elevator->D2 = 1;

    // 如果当前在本垒层则继续保持
    if (elevator->curfloor == 1)
    {
        elevator->movement = Waiting;
        printf("Elevator is waiting at 1 floor\n");
        elevator->State = Idle;
        elevator->movementTim = waitTIM;
    }
    // 如果在本垒层下方则准备回到一楼
    else if (elevator->curfloor < 1)
    {
        // 设置电梯加速启动
        elevator->movement = Accelerating;
        // 计时
        elevator->movementTim = accelerateTIM;
        // 上楼
        elevator->State = GoingUp;
    }
    // 如果在本垒层上方则准备回到一楼
    else
    {
        // 设置电梯加速启动
        elevator->movement = Accelerating;
        // 计时
        elevator->movementTim = accelerateTIM;
        // 下楼
        elevator->State = GoingDown;
    }
    return OK;
}

/**
 * @brief 取消D2标记
 */
Status CancelD2(Elevator *elevator)
{
    elevator->D2 = 0;
    return OK;
}

/*****************************************************************************************/

/* Queue */
/******************************************************************************************/
/**
 * 
 */
Status InitQueue(WaitQueue *Q)
{
    // 分配内存空间
    Q->front = Q->rear = (ClientQueueNode *)malloc(sizeof(ClientQueueNode));
    Q->front->next = NULL;
    // 初始化等待数量为0
    Q->WaitNum = 0;
    return OK;
}

Status Enqueue(WaitQueue *Q, Client temp)
{
    // 分配新的内存空间
    ClientQueueNode *ptr = (ClientQueueNode *)malloc(sizeof(ClientQueueNode));

    ptr->passenger = temp;
    ptr->next = NULL;
    Q->rear->next = ptr;
    Q->rear = ptr;
    Q->WaitNum++;
    return OK;
}

/**
 * 
 */
Status Dequeue(WaitQueue *Q, Client *temp)
{
    // 如果队列为空直接返回
    if (Q->front == Q->rear)
    {
        return Error;
    }

    // 删除元素
    ClientQueueNode *ptr = Q->front->next;
    Q->front->next = ptr->next;

    *temp = ptr->passenger;
    free(ptr);
    Q->WaitNum--;
    return OK;
}

Status InitWaitQueue()
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            InitQueue(&Queue[i][j]);
        }
    }
}
/**
 * @brief 更新CallUp和CallDown数组
 */
void Update(void)
{
    // 遍历等待队列
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            // i等于0代表上楼等待队列
            if (i == 0)
            {
                // 如果没人等待CallUp就设置为0
                if (Queue[i][j].WaitNum == 0)
                {
                    CallUp[j] = 0;
                }
            }
            // i等于1代表下楼等待队列
            else
            {
                // 如果没人等待CallDown就设置为0
                if (Queue[i][j].WaitNum == 0)
                {
                    CallDown[j] = 0;
                }
            }
        }
    }
    return;
}

/*****************************************************************************************/

/* Stack */
/******************************************************************************************/

/**
 * @brief 乘客出电梯栈
 */
Status Pop(ClientStack *S, Client *temp)
{
    *temp = S->base[S->top--];
    return OK;
}

/**
 * @brief 乘客进电梯栈
 */
Status Push(ClientStack *S, Client temp)
{
    if (S->top < MaxNum)
    {
        S->base[++S->top] = temp;
    }
    return OK;
}

/**
 * @brief 判断电梯栈是否为空
 */
Status StackEmpty(ClientStack S)
{
    // 如果栈顶为-1返回True
    if (S.top == -1)
    {
        return True;
    }
    // 否则返回False
    return False;
}

/*****************************************************************************************/

/* Client */
/******************************************************************************************/

/**
 * @brief 产生新乘客并入队
 */
Status Newclient()
{
    Client NewClient;
    NewClient.occurTime = Time;                     // 新乘客到达时间
    NewClient.ClientID = ++ID;                      // 新乘客的ID
    NewClient.Infloor = rand() % 5;                 // 新乘客起始楼层
    NewClient.OutFloor = rand() % 5;                // 新乘客目标楼层
    NewClient.GiveupTime = 100 + rand() % 300;      // 最长等待时间
    InterTime = rand() % 80;                        // 下一个乘客到达时间间隔
    while (NewClient.Infloor == NewClient.OutFloor) // 如果新乘客出发地和目标地相同，重新分配
    {
        NewClient.OutFloor = rand() % 5;
    }

    // 如果是上楼则进入对应的上楼等待队列
    if (NewClient.Infloor < NewClient.OutFloor)
    {
        Enqueue(&Queue[0][NewClient.Infloor], NewClient);
        printf("No.%d Client gets into the %d floor's %d waitqueue(0-->UP,1-->DOWN)\n", NewClient.ClientID, NewClient.Infloor, NewClient.Infloor > NewClient.OutFloor);
        // 上楼呼叫
        CallUp[NewClient.Infloor] = 1;
    }
    // 如果是下楼则进入对应的下楼等待队列
    else
    {
        Enqueue(&Queue[1][NewClient.Infloor], NewClient);
        printf("No.%d Client gets into the %d floor's %d waitqueue(0-->UP,1-->DOWN)\n", NewClient.ClientID, NewClient.Infloor, NewClient.Infloor > NewClient.OutFloor);
        // 下楼呼叫
        CallDown[NewClient.Infloor] = 1;
    }
}

/**
 * @brief 删除耐心到达极限的乘客朋友
 */
Status ClientGiveUp()
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            WaitQueue temp = Queue[i][j];
            ClientQueueNode *ptr = temp.front->next;
            ClientQueueNode *pre = temp.front;
            while (ptr != NULL)
            {
                // 如果超过等待限度
                if (ptr->passenger.occurTime + ptr->passenger.GiveupTime <= Time)
                {
                    // 乘客出列
                    pre->next = ptr->next;
                    // 打印提示消息
                    printf("No.%d Client leave the %d floor %d waitqueue\n", ptr->passenger.ClientID, ptr->passenger.Infloor, i);
                    // 释放节点内存
                    free(ptr);
                    ptr = pre->next;
                    // 排队人数减少
                    Queue[i][j].WaitNum--;
                }
                else
                {
                    pre = ptr;
                    ptr = ptr->next;
                }
            }
        }
    }
    // 更新CallUp和CallDown数组
    Update();
}

/**
 * @brief 乘客出电梯栈
 */
Status ClientOut(Elevator *elevator)
{
    // 当前楼层的乘客栈不空,则清空乘客栈
    if (elevator->Stack[elevator->curfloor].top != -1)
    {
        Client temp;
        // 乘客出栈
        Pop(&elevator->Stack[elevator->curfloor], &temp);
        // 打印提示消息
        printf("No.%d Client leave the elevator at %d floor\n", temp.ClientID, temp.OutFloor);
        // 乘客数量减少
        elevator->ClientNum--;

        // 时间增加
        Time += PeopleInAndOutTIM * t;
    }
    // 更新CallUp和CallDown数组
    Update();
}

/**
 * @brief 判断电梯行进方向上是否有乘客等待
 */
Status IfPeopleIn(Elevator *elevator)
{
    // 如果电梯向上
    if (elevator->State == GoingUp)
    {
        // 有人等待返回True
        if (CallUp[elevator->curfloor])
        {
            return True;
        }
        // 如果电梯已经等待300s则有人等待就返回True
        if (!elevator->ClientNum && CallDown[elevator->curfloor])
        {
            return True;
        }
    }
    // 如果电梯向下
    else if (elevator->State == GoingDown)
    {
        // 如果有人等待返回True
        if (CallDown[elevator->curfloor])
        {
            return True;
        }
        // 如果上一个状态为Idle则有人等待就返回True
        if (!elevator->ClientNum && CallUp[elevator->curfloor])
        {
            return True;
        }
    }
    else
    {
        if (CallDown[elevator->curfloor] || CallUp[elevator->curfloor])
        {
            return True;
        }
    }

    // 没人等待返回False
    return False;
}

/**
 * @brief 乘客进入电梯栈并且设置CallCar
 */
Status ClientIn(Elevator *elevator)
{
    // 如果电梯没人,不管向上向下都接收
    if (!elevator->ClientNum)
    {
        // 为了简便,这里优先考虑上楼队列
        if (CallUp[elevator->curfloor])
        {
            // 从等待队列中取出乘客
            Client temp;
            Dequeue(&Queue[0][elevator->curfloor], &temp);
            // 乘客入栈
            Push(&elevator->Stack[temp.OutFloor], temp);
            // 设置目的地
            elevator->CallCar[temp.OutFloor] = 1;
            // 入栈提示
            printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
        }
        else if (CallDown[elevator->curfloor])
        {
            // 从等待队列中取出乘客
            Client temp;
            Dequeue(&Queue[1][elevator->curfloor], &temp);
            // 乘客入栈
            Push(&elevator->Stack[temp.OutFloor], temp);
            // 设置目的地
            elevator->CallCar[temp.OutFloor] = 1;
            // 入栈提示
            printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
        }
    }

    else if (elevator->State == GoingUp)
    {
        // 如果上一状态不是空闲的,那么只能接与行进状态相同的乘客

        // 从等待队列中取出乘客
        Client temp;
        Dequeue(&Queue[0][elevator->curfloor], &temp);
        // 乘客入栈
        Push(&elevator->Stack[temp.OutFloor], temp);
        // 设置目的地
        elevator->CallCar[temp.OutFloor] = 1;
        // 入栈提示
        printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
    }
    else if (elevator->State == GoingDown)
    {

        // 从等待队列中取出乘客
        Client temp;
        Dequeue(&Queue[1][elevator->curfloor], &temp);
        // 乘客入栈
        Push(&elevator->Stack[temp.OutFloor], temp);
        // 设置目的地
        elevator->CallCar[temp.OutFloor] = 1;
        // 入栈提示
        printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
    }
    // 电梯乘客增加
    elevator->ClientNum++;
    return OK;
}

/*****************************************************************************************/

int main(int argc, char const *argv[])
{

    srand(time(NULL));

    Elevator elevator;

    InitElevator(&elevator);
    InitWaitQueue();

    while (Time++ < MaxTime)
    {
        // 检查是否有乘客放弃排队
        ClientGiveUp();
        // 如果InterTime为0则加入新乘客
        if (!InterTime)
        {
            Newclient();
        }
        // 否则InterTime自减
        else
        {
            InterTime--;
        }

        // 电梯模拟调度
        Controller(&elevator);

        // 正在关门或者等待时本层出现乘客则进入电梯
        if (elevator.movement == Closing)
        {
            // 如果当前电梯不为空,如果有相同行进方向的乘客则入栈
            if (elevator.ClientNum && elevator.ClientNum < MaxNum)
            {
                // 如果当前电梯向上,有向上的乘客则切换到开门
                if (elevator.State == GoingUp)
                {
                    if (CallUp[elevator.curfloor])
                    {
                        elevator.movement = Opening;
                        elevator.movementTim = doorTIM;
                    }
                }
                // 如果当前电梯向下,有向下的乘客则切换到开门
                else if (elevator.State == GoingDown)
                {
                    if (CallDown[elevator.curfloor])
                    {
                        elevator.movement = Opening;
                        elevator.movementTim = doorTIM;
                    }
                }
            }
            // 如果当前电梯为空
            else if (!elevator.ClientNum)
            {
                // 只要有请求则都切换到开门状态
                if (CallUp[elevator.curfloor] || CallDown[elevator.curfloor])
                {
                    elevator.movement = Opening;
                    elevator.movementTim = doorTIM;
                }
            }
        }
        else if (elevator.movement == Waiting)
        {
            int j = HigherRequests(&elevator);
            int i = LowerRequests(&elevator);
            // 如果请求在本层则切换到开门
            if (j == elevator.curfloor)
            {
                elevator.movement = Opening;
                elevator.movementTim = doorTIM;
            }
            // 如果请求在高层则切换到加速并把State设置为GoingUp
            else if (j != -1)
            {
                elevator.movement = Accelerating;
                elevator.movementTim = accelerateTIM;
                elevator.State = GoingUp;
            }
            // 如果请求在低层则切换到加速并把State设置为GoingDown
            else if (i != -1)
            {
                elevator.movement = Accelerating;
                elevator.movementTim = accelerateTIM;
                elevator.State = GoingDown;
            }
        }

        // 电梯当前行为剩余的时间如果为0
        if (!elevator.movementTim)
        {
            ChangeMovement(&elevator);
        }
        // 如果不为0,则计时减小
        else
        {
            elevator.movementTim--;
        }
    }
    return 0;
}
