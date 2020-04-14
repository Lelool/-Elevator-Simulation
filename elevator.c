#include "elevator.h"

/* Elevator */
/*****************************************************************************************/
Status InitElevator(Elevator *elevator)
{
    // ��ʼ��CallCar�Լ�CallUp��CallDown
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

    // ��¼��Ҫȥ����һ��¥
    int nextfloor = -1;

    // ���¥��¥�¶���������ѡ�����������
    if (highrequest != -1 && lowrequest != -1)
    {
        // ���¥���������,��ônextfloor����¥������
        if (highrequest - elevator->curfloor >= elevator->curfloor - lowrequest)
        {
            nextfloor = lowrequest;
        }
        // ���¥���������,��ônextfloor����¥������
        else
        {
            nextfloor = highrequest;
        }
    }
    // ���¥��û����,¥��������
    else if (highrequest == -1)
    {
        // nextfloor����¥������
        nextfloor = lowrequest;
    }
    // ���¥��û����,¥��������
    else if (lowrequest == -1)
    {
        // nextfloor����¥������
        nextfloor = highrequest;
    }

    // ���û������ֱ�ӷ���
    if (nextfloor == -1)
    {
        return OK;
    }
    // ����Ǳ���������,�л�״̬Ϊ���ڿ���,��������TIM
    else if (nextfloor == elevator->curfloor)
    {
        // ���ÿ���״̬
        elevator->movement = Opening;
        // ����TIM
        elevator->movementTim = doorTIM;
    }
    // ���¥������
    else if (nextfloor < elevator->curfloor)
    {
        // ���õ���״̬Ϊ�½�
        elevator->State = GoingDown;
        // ��ǰ��ΪΪ���ڼ���
        elevator->movement = Accelerating;
        // ����TIM
        elevator->movementTim = accelerateTIM;
    }
    // �����¥������
    else
    {
        // ��������״̬
        elevator->State = GoingUp;
        // ������Ϊ
        elevator->movement = Accelerating;
        // ����ʱ��
        elevator->movementTim = accelerateTIM;
    }
}

/**
 * @brief �ı�����˶�״̬
 */
Status ChangeMovement(Elevator *elevator)
{
    switch (elevator->movement)
    {
    // ���Ž����л���Opened
    case Opening:
        elevator->movement = Opened;
        printf("Opened the door successfully!\n");
        elevator->movementTim = testTIM;
        break;

    // �˿ͽ���,��ɺ��л���Closing
    case Opened:

        // �������ջ������˿ͳ�����
        if (StackEmpty(elevator->Stack[elevator->curfloor]) == False)
        {
            // �˿ͳ�ջ
            ClientOut(elevator);
            // ��ʱ
            elevator->movementTim = PeopleInAndOutTIM;
        }
        // �˿Ͷ���ջ�������һ���ж�
        else
        {
            // ���µ�ǰ¥��CallCar����
            elevator->CallCar[elevator->curfloor] = 0;
            // �����ǰ�н����������˵ȴ����ҵ���δ��
            int retval = IfPeopleIn(elevator);
            if (retval == True && elevator->ClientNum < MaxNum)
            {
                // �˿���ջ
                ClientIn(elevator);
                // ��ʱ
                elevator->movementTim = PeopleInAndOutTIM;
                // ȡ��D2
                CancelD2(elevator);
            }
            // ���û�˵ȴ���ô������һ��״̬
            else
            {
                elevator->movement = Closing;
                printf("Now the elevator is closing the door.\n");
                elevator->movementTim = doorTIM;
            }
        }

        break;

    // ���Ž������л���Closed
    case Closing:
        // ����
        elevator->movement = Closed;
        // ��ӡ��Ϣ
        printf("Closed the door successfully!\n");
        break;

    // ���ź��л����������������ΪAccelerating�����л���Waiting
    case Closed:
        // ���������,���õ���״̬Ϊ���ڼ���
        if (HigherRequests(elevator) != -1 || LowerRequests(elevator) != -1)
        {
            elevator->movement = Accelerating;
            // ��ʱ
            elevator->movementTim = accelerateTIM;
            // ��ӡ��ʾ��Ϣ
            printf("Elevator is accelerating.\n");
        }
        // �Ϸ����������������ݷ�����ͬ,�������ͬ���ҵ����ڻ��г˿�,�͸ı��˶�״̬
        else if (elevator->ClientNum)
        {
            ChangeState(elevator);
        }
        // ���û��������û�����Ǿ͵ȴ�
        else
        {
            elevator->movement = Waiting;
            elevator->movementTim = waitTIM;
            // ��ʾ��Ϣ
            printf("Elevator is waiting at %d floor\n", elevator->curfloor);
        }

        break;

    // �ж��Ƿ�Ӧ�ü����ƶ����߼���
    case Moving:
        // D2Ϊ1��ʾ�������ڷ��ر��ݲ�
        if (elevator->D2)
        {
            // �����ǰ¥���ڱ��ݲ��·�
            if (elevator->curfloor < 1)
            {
                // ��¥
                Upstairs(elevator);
                // �����ƶ�
                elevator->movement = Moving;
                // ��ʱ
                elevator->movementTim = upstairsTIM;
            }
            // �����ǰ¥���ڱ��ݲ��Ϸ�
            else if (elevator->curfloor > 1)
            {
                // ��¥
                DownStairs(elevator);
                // �����ƶ�
                elevator->movement = Moving;
                // ��ʱ
                elevator->movementTim = downstairsTIM;
            }
            // ��������˱��ݲ�
            else
            {
                // ����ɲ��
                elevator->movement = SlowingDown;
                // ��ʱ
                elevator->movementTim = slowdownTIM;
            }
        }
        else
        {
            // ��¼nextfloor��ֵ
            int nextfloor;
            // �������������¥�ͻ�ȡ¥�ϵ�����
            if (elevator->State == GoingUp)
            {
                nextfloor = HigherRequests(elevator);
            }
            // �������������¥�ͻ�ȡ¥�µ�����
            else
            {
                nextfloor = LowerRequests(elevator);
            }
            // �����ǰ¥��,����ݽ���ɲ��
            if (nextfloor == elevator->curfloor)
            {
                elevator->movement = SlowingDown;
                elevator->movementTim = slowdownTIM;
            }
            // ���Ӧ�ü���������ô���µ�ǰ״̬��ʱ�䲢��¥
            else if (nextfloor < elevator->curfloor)
            {
                DownStairs(elevator);
                elevator->movement = Moving;
                elevator->movementTim = downstairsTIM;
            }
            // ���Ӧ�ü���������ô���µ�ǰ״̬��ʱ�䲢��¥
            else
            {
                Upstairs(elevator);
                elevator->movement = Moving;
                elevator->movementTim = upstairsTIM;
            }
        }

        break;

    // �ȴ���ɺ�ת�����ݲ�
    case Waiting:
        // �ȴ�����300s��������ڱ��ݲ��׼�����ر��ݲ�
        // �����ȴ�ʱ���Ѿ�����300s

        // ������ڱ��ݲ��򷵻ر��ݲ�,����ڱ��ݲ��򱣳���״
        BackToBase(elevator);

        break;

    // ������ɺ��л���Moving
    case Accelerating:

        // �������֮����������ƶ�
        elevator->movement = Moving;
        // �������������¥
        if (elevator->State == GoingDown)
        {
            // ��ʱΪ��¥��ʱ
            elevator->movementTim = downstairsTIM;
        }
        // �������������¥
        else if (elevator->State == GoingUp)
        {
            // ��ʱΪ��¥��ʱ
            elevator->movementTim = upstairsTIM;
        }

        break;

    // ������ɺ��л���Opening
    case SlowingDown:
        // �������������,��һ�����ǿ���
        if (!elevator->D2)
        {
            // �������֮����
            elevator->movement = Opening;
            // ��ʱ
            elevator->movementTim = doorTIM;
        }
        // ����ص����ݲ�ļ���,���л�Ϊ�ȴ�
        else
        {
            elevator->movement = Waiting;
            elevator->movementTim = waitTIM;
        }

        break;
    }
}

/**
 * @brief  ����¥������
 * @retval ����������򷵻���ӽ���ǰ¥�������,���򷵻�-1
 */
int HigherRequests(Elevator *elevator)
{
    // ����״̬��ΪGoingUp����Wating���ܿ�����¥
    if (elevator->State == GoingUp || elevator->movement == Waiting)
    {
        // �ҵ���ӽ�������
        for (int i = elevator->curfloor; i < 5; i++)
        {
            if (elevator->CallCar[i] || CallUp[i] || CallDown[i])
            {
                // ������ӽ�������¥��
                return i;
            }
        }
    }
    return -1;
}

/**
 * @brief  ����¥������
 * @retval ����������򷵻����¥������,���򷵻�-1
 */
int LowerRequests(Elevator *elevator)
{
    // ����״̬��ΪGoingDown����Waiting����
    if (elevator->State == GoingDown || elevator->movement == Waiting)
    {
        // Ѱ���������
        for (int i = elevator->curfloor; i >= 0; i--)
        {
            if (elevator->CallCar[i] || CallUp[i] || CallDown[i])
            {
                // ���������¥��
                return i;
            }
        }
    }
    // û�ҵ��򷵻�-1
    return -1;
}

/**
 * @brief �ı���ݵ�״̬
 */
void ChangeState(Elevator *elevator)
{
    if (elevator->ClientNum >= MaxNum)
        return;
    int temp = 0;
    // ��¥���Ƿ�������
    for (int i = elevator->curfloor + 1; i < 5; i++)
    {
        if (elevator->CallCar[i] || CallUp[i])
        {
            temp = 1;
        }
    }
    // ���¥��û�������ҵ�ǰ�������г˿�,��ô����State��Ҫ�ı�
    if (elevator->ClientNum && !temp)
    {
        elevator->State = GoingDown;
    }

    temp = 0;
    // ��¥���Ƿ�������
    for (int i = elevator->curfloor - 1; i >= 0; i--)
    {
        if (elevator->CallCar[i] || CallDown[i])
        {
            temp = 1;
        }
    }
    // ���¥��û�������ҵ�ǰ�������г˿�,��ô����State��Ҫ�ı�
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
 * @brief ���ص����ݲ�
 */
Status BackToBase(Elevator *elevator)
{
    // ��ʾ�ȴ�ʱ���Ѿ�����300s
    elevator->D2 = 1;

    // �����ǰ�ڱ��ݲ����������
    if (elevator->curfloor == 1)
    {
        elevator->movement = Waiting;
        printf("Elevator is waiting at 1 floor\n");
        elevator->State = Idle;
        elevator->movementTim = waitTIM;
    }
    // ����ڱ��ݲ��·���׼���ص�һ¥
    else if (elevator->curfloor < 1)
    {
        // ���õ��ݼ�������
        elevator->movement = Accelerating;
        // ��ʱ
        elevator->movementTim = accelerateTIM;
        // ��¥
        elevator->State = GoingUp;
    }
    // ����ڱ��ݲ��Ϸ���׼���ص�һ¥
    else
    {
        // ���õ��ݼ�������
        elevator->movement = Accelerating;
        // ��ʱ
        elevator->movementTim = accelerateTIM;
        // ��¥
        elevator->State = GoingDown;
    }
    return OK;
}

/**
 * @brief ȡ��D2���
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
    // �����ڴ�ռ�
    Q->front = Q->rear = (ClientQueueNode *)malloc(sizeof(ClientQueueNode));
    Q->front->next = NULL;
    // ��ʼ���ȴ�����Ϊ0
    Q->WaitNum = 0;
    return OK;
}

Status Enqueue(WaitQueue *Q, Client temp)
{
    // �����µ��ڴ�ռ�
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
    // �������Ϊ��ֱ�ӷ���
    if (Q->front == Q->rear)
    {
        return Error;
    }

    // ɾ��Ԫ��
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
 * @brief ����CallUp��CallDown����
 */
void Update(void)
{
    // �����ȴ�����
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            // i����0������¥�ȴ�����
            if (i == 0)
            {
                // ���û�˵ȴ�CallUp������Ϊ0
                if (Queue[i][j].WaitNum == 0)
                {
                    CallUp[j] = 0;
                }
            }
            // i����1������¥�ȴ�����
            else
            {
                // ���û�˵ȴ�CallDown������Ϊ0
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
 * @brief �˿ͳ�����ջ
 */
Status Pop(ClientStack *S, Client *temp)
{
    *temp = S->base[S->top--];
    return OK;
}

/**
 * @brief �˿ͽ�����ջ
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
 * @brief �жϵ���ջ�Ƿ�Ϊ��
 */
Status StackEmpty(ClientStack S)
{
    // ���ջ��Ϊ-1����True
    if (S.top == -1)
    {
        return True;
    }
    // ���򷵻�False
    return False;
}

/*****************************************************************************************/

/* Client */
/******************************************************************************************/

/**
 * @brief �����³˿Ͳ����
 */
Status Newclient()
{
    Client NewClient;
    NewClient.occurTime = Time;                     // �³˿͵���ʱ��
    NewClient.ClientID = ++ID;                      // �³˿͵�ID
    NewClient.Infloor = rand() % 5;                 // �³˿���ʼ¥��
    NewClient.OutFloor = rand() % 5;                // �³˿�Ŀ��¥��
    NewClient.GiveupTime = 100 + rand() % 300;      // ��ȴ�ʱ��
    InterTime = rand() % 80;                        // ��һ���˿͵���ʱ����
    while (NewClient.Infloor == NewClient.OutFloor) // ����³˿ͳ����غ�Ŀ�����ͬ�����·���
    {
        NewClient.OutFloor = rand() % 5;
    }

    // �������¥������Ӧ����¥�ȴ�����
    if (NewClient.Infloor < NewClient.OutFloor)
    {
        Enqueue(&Queue[0][NewClient.Infloor], NewClient);
        printf("No.%d Client gets into the %d floor's %d waitqueue(0-->UP,1-->DOWN)\n", NewClient.ClientID, NewClient.Infloor, NewClient.Infloor > NewClient.OutFloor);
        // ��¥����
        CallUp[NewClient.Infloor] = 1;
    }
    // �������¥������Ӧ����¥�ȴ�����
    else
    {
        Enqueue(&Queue[1][NewClient.Infloor], NewClient);
        printf("No.%d Client gets into the %d floor's %d waitqueue(0-->UP,1-->DOWN)\n", NewClient.ClientID, NewClient.Infloor, NewClient.Infloor > NewClient.OutFloor);
        // ��¥����
        CallDown[NewClient.Infloor] = 1;
    }
}

/**
 * @brief ɾ�����ĵ��Ｋ�޵ĳ˿�����
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
                // ��������ȴ��޶�
                if (ptr->passenger.occurTime + ptr->passenger.GiveupTime <= Time)
                {
                    // �˿ͳ���
                    pre->next = ptr->next;
                    // ��ӡ��ʾ��Ϣ
                    printf("No.%d Client leave the %d floor %d waitqueue\n", ptr->passenger.ClientID, ptr->passenger.Infloor, i);
                    // �ͷŽڵ��ڴ�
                    free(ptr);
                    ptr = pre->next;
                    // �Ŷ���������
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
    // ����CallUp��CallDown����
    Update();
}

/**
 * @brief �˿ͳ�����ջ
 */
Status ClientOut(Elevator *elevator)
{
    // ��ǰ¥��ĳ˿�ջ����,����ճ˿�ջ
    if (elevator->Stack[elevator->curfloor].top != -1)
    {
        Client temp;
        // �˿ͳ�ջ
        Pop(&elevator->Stack[elevator->curfloor], &temp);
        // ��ӡ��ʾ��Ϣ
        printf("No.%d Client leave the elevator at %d floor\n", temp.ClientID, temp.OutFloor);
        // �˿���������
        elevator->ClientNum--;

        // ʱ������
        Time += PeopleInAndOutTIM * t;
    }
    // ����CallUp��CallDown����
    Update();
}

/**
 * @brief �жϵ����н��������Ƿ��г˿͵ȴ�
 */
Status IfPeopleIn(Elevator *elevator)
{
    // �����������
    if (elevator->State == GoingUp)
    {
        // ���˵ȴ�����True
        if (CallUp[elevator->curfloor])
        {
            return True;
        }
        // ��������Ѿ��ȴ�300s�����˵ȴ��ͷ���True
        if (!elevator->ClientNum && CallDown[elevator->curfloor])
        {
            return True;
        }
    }
    // �����������
    else if (elevator->State == GoingDown)
    {
        // ������˵ȴ�����True
        if (CallDown[elevator->curfloor])
        {
            return True;
        }
        // �����һ��״̬ΪIdle�����˵ȴ��ͷ���True
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

    // û�˵ȴ�����False
    return False;
}

/**
 * @brief �˿ͽ������ջ��������CallCar
 */
Status ClientIn(Elevator *elevator)
{
    // �������û��,�����������¶�����
    if (!elevator->ClientNum)
    {
        // Ϊ�˼��,�������ȿ�����¥����
        if (CallUp[elevator->curfloor])
        {
            // �ӵȴ�������ȡ���˿�
            Client temp;
            Dequeue(&Queue[0][elevator->curfloor], &temp);
            // �˿���ջ
            Push(&elevator->Stack[temp.OutFloor], temp);
            // ����Ŀ�ĵ�
            elevator->CallCar[temp.OutFloor] = 1;
            // ��ջ��ʾ
            printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
        }
        else if (CallDown[elevator->curfloor])
        {
            // �ӵȴ�������ȡ���˿�
            Client temp;
            Dequeue(&Queue[1][elevator->curfloor], &temp);
            // �˿���ջ
            Push(&elevator->Stack[temp.OutFloor], temp);
            // ����Ŀ�ĵ�
            elevator->CallCar[temp.OutFloor] = 1;
            // ��ջ��ʾ
            printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
        }
    }

    else if (elevator->State == GoingUp)
    {
        // �����һ״̬���ǿ��е�,��ôֻ�ܽ����н�״̬��ͬ�ĳ˿�

        // �ӵȴ�������ȡ���˿�
        Client temp;
        Dequeue(&Queue[0][elevator->curfloor], &temp);
        // �˿���ջ
        Push(&elevator->Stack[temp.OutFloor], temp);
        // ����Ŀ�ĵ�
        elevator->CallCar[temp.OutFloor] = 1;
        // ��ջ��ʾ
        printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
    }
    else if (elevator->State == GoingDown)
    {

        // �ӵȴ�������ȡ���˿�
        Client temp;
        Dequeue(&Queue[1][elevator->curfloor], &temp);
        // �˿���ջ
        Push(&elevator->Stack[temp.OutFloor], temp);
        // ����Ŀ�ĵ�
        elevator->CallCar[temp.OutFloor] = 1;
        // ��ջ��ʾ
        printf("No.%d Client get into the elevator at %d floor.\n", temp.ClientID, elevator->curfloor);
    }
    // ���ݳ˿�����
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
        // ����Ƿ��г˿ͷ����Ŷ�
        ClientGiveUp();
        // ���InterTimeΪ0������³˿�
        if (!InterTime)
        {
            Newclient();
        }
        // ����InterTime�Լ�
        else
        {
            InterTime--;
        }

        // ����ģ�����
        Controller(&elevator);

        // ���ڹ��Ż��ߵȴ�ʱ������ֳ˿���������
        if (elevator.movement == Closing)
        {
            // �����ǰ���ݲ�Ϊ��,�������ͬ�н�����ĳ˿�����ջ
            if (elevator.ClientNum && elevator.ClientNum < MaxNum)
            {
                // �����ǰ��������,�����ϵĳ˿����л�������
                if (elevator.State == GoingUp)
                {
                    if (CallUp[elevator.curfloor])
                    {
                        elevator.movement = Opening;
                        elevator.movementTim = doorTIM;
                    }
                }
                // �����ǰ��������,�����µĳ˿����л�������
                else if (elevator.State == GoingDown)
                {
                    if (CallDown[elevator.curfloor])
                    {
                        elevator.movement = Opening;
                        elevator.movementTim = doorTIM;
                    }
                }
            }
            // �����ǰ����Ϊ��
            else if (!elevator.ClientNum)
            {
                // ֻҪ���������л�������״̬
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
            // ��������ڱ������л�������
            if (j == elevator.curfloor)
            {
                elevator.movement = Opening;
                elevator.movementTim = doorTIM;
            }
            // ��������ڸ߲����л������ٲ���State����ΪGoingUp
            else if (j != -1)
            {
                elevator.movement = Accelerating;
                elevator.movementTim = accelerateTIM;
                elevator.State = GoingUp;
            }
            // ��������ڵͲ����л������ٲ���State����ΪGoingDown
            else if (i != -1)
            {
                elevator.movement = Accelerating;
                elevator.movementTim = accelerateTIM;
                elevator.State = GoingDown;
            }
        }

        // ���ݵ�ǰ��Ϊʣ���ʱ�����Ϊ0
        if (!elevator.movementTim)
        {
            ChangeMovement(&elevator);
        }
        // �����Ϊ0,���ʱ��С
        else
        {
            elevator.movementTim--;
        }
    }
    return 0;
}
