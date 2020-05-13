#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <Windows.h>
using namespace std;

//进程状态
enum States { Ready, Run, Block, Done };
//信号量
enum BlockReason { S1, S2 };
//PCB的结构体
struct PCB
{
	string name;
	States state;
	BlockReason reason;
	int BreakPoint;
};

const int BufferLength = 10;
//缓冲区
int buffer[BufferLength];
int in, out;
int x = 0;
//进程的指令序列
int PA[5] = { 4,0,2,1,6 };
int SA[5] = { 0,3,1,5,6 };
int PC;//模拟调度中的PC
PCB Producer, Consumer;//Producer和Consumer进程
PCB* CurrentProcess;
int production;//production初始为0
int s1, s2;//s1和s2为信号量，初始时s1为10，s2为0
int max;//最大生产数

void p()
{
	if (CurrentProcess == &Producer)
	{
		cout << "生产者P操作" << endl;
		if (--s1 < 0)//将信号量s减去1，若结果小于0，则执行原语的进程被置成等待信号量s的状态
		{
			cout << "生产者等待S1,生产者阻塞" << endl;
			CurrentProcess->state = Block;
			CurrentProcess->reason = S1;
			CurrentProcess->BreakPoint = 0;
		}
		else
			cout << "继续" << endl;
	}
	else
	{
		cout << "消费者P操作" << endl;
		if (--s2 < 0)//将信号量s减去1，若结果小于0，则执行原语的进程被置成等待信号量s的状态
		{
			cout << "消费者等待S2，消费者阻塞" << endl;
			CurrentProcess->state = Block;
			CurrentProcess->reason = S2;
			CurrentProcess->BreakPoint = 0;
		}
		else
			cout << "继续" << endl;
	}
}

void v()
{
	if (CurrentProcess == &Producer)
	{
		cout << "生产者V操作" << endl;
		if (++s2 <= 0 && Consumer.state == Block)//将信号量s加1，若结果不大于0，则释放一个等待信号量s的进程
		{
			Consumer.state = Ready;
			cout << "消费者就绪" << endl;
		}

	}
	else
	{
		cout << "消费者V操作" << endl;
		if (++s1 <= 0 && Producer.state == Block)//将信号量s加1，若结果不大于0，则释放一个等待信号量s的进程
		{
			Producer.state = Ready;
			cout << "生产者就绪" << endl;
		}
	}
}

void put()
{
	buffer[in] = production;
	cout << "生产者PUT  " << production << endl;
	in = (in + 1) % BufferLength;
}

void get()
{
	x = buffer[out];
	cout << "消费者GET  " << x << endl;
	out = (out + 1) % BufferLength;
}

void produce()
{
	production++;
	cout << "生产产品  " << production <<endl;
}

void consume()
{
	cout << "消费产品  " << x << endl;
}

void goto0()
{
	if (CurrentProcess == &Producer)
	{
		cout << "生产者 GOTO 0" << endl;
		if (in >= max)
		{
			cout << "生产者可生产产品数已达最大，生产者运行结束。" << endl;
			CurrentProcess->state = Done;
		}
	}
	else
		cout << "消费者GOTO 0" << endl;
	PC = 0;
}

void Initiate()//初始化
{
	s1 = BufferLength;
	s2 = 0;
	Producer.name = "producer";
	Consumer.name = "consumer";
	Producer.state = Consumer.state = Ready;
	Producer.BreakPoint = Consumer.BreakPoint = 0;
	CurrentProcess = &Producer;
	PC = 0;
	in = out = 0;
	production = 0;
	srand((unsigned int)time(NULL));
}

void Execute()
{
	int i = PC++;
	int j = (CurrentProcess == &Producer) ? PA[i] : SA[i];

	switch (j)
	{
	case 0: p(); break;
	case 1: v(); break;
	case 2: put(); break;
	case 3: get(); break;
	case 4: produce(); break;
	case 5: consume(); break;
	case 6: goto0(); break;
	default: break;//空操作，即NOP
	}
}

void Dispatch()
{
	while (true)
	{
		//将当前状态设为就绪，保护现场
		CurrentProcess->BreakPoint = PC;
		if (CurrentProcess->state != Block && CurrentProcess->state != Done)
			CurrentProcess->state = Ready;
		//从就绪队列中按随机数选一个出来
	
		if (Producer.state == Ready && Consumer.state == Ready)
			CurrentProcess = (rand() % 2 == 0) ? &Producer : &Consumer;

		else if (Producer.state == Ready)//生产者就绪
			CurrentProcess = &Producer;
		else if (Consumer.state == Ready)//消费者就绪
			CurrentProcess = &Consumer;

		else//无就绪进程
		{
			cout << "就绪队列为空，调度结束" << endl;
			exit(0);
		}
		//将现行进程状态改为运行态
		CurrentProcess->state = Run;
		PC = CurrentProcess->BreakPoint;
		Execute();
	}
}
int main()
{
	cout << "生产者可生产的最大产品数：" << endl;//当到达生产者可生产的最大商品数量时，生产者运行结束（为人工选择的方法）
	cin >> max;
	Initiate();//初始化
	Dispatch();//运行调度
	return 0;
}
