#ifndef RAMMODEL_H
#define RAMMODEL_H

#include <vector>
#include <QList>
#include <list>
#include <QDebug>

//数据源的来源 ： 默认和手动输入
enum data_source {
    DEFAULT,
    CUSTOMIZE
};

//两种算法的选择，首次适应和最佳适应
enum algorithm {
    FIRST_FIT,
    BEST_FIT
};

//默认的申请队列
const int default_data[11][2] = {
    {1, -130},
    {2, -60},
    {3, -100},
    {2, +60},
    {4, -200},
    {3, +100},
    {1, +130},
    {5, -140},
    {6, -60},
    {7, -50},
    {6, +60}
};

typedef struct a {
    int start_addr;
    int lenth;
} ram_block;

typedef struct b {
    int job_num;
    ram_block block_in_ram;
} job;

typedef struct c {
    int job_num;
    int apply_or_release;
    bool request_state;
    int lenth;
} request_inf;//发出请求之后的日志信息

class RAMModel
{
public:
    RAMModel();
    ~RAMModel();

    std::vector<job> jobs_in_ram;//在内存中的作业信息，用于在主界面中绘制

    int current_job_num;//当前申请的作业号 在自定义的模式中使用


    bool apply_ram(int job_num, int lenth);//申请

//    bool apply_ram();//默认情况使用

    bool release_ram(int job_num);//释放
//    bool release_ram();

    void setting(data_source source, algorithm al, int capacity);//设置函数

    request_inf next_step();//默认数据情况下的下一步

    //判断是否还有剩余的数据
    bool has_left_default_data(){
        if(current_request > 10)
            return false;
        else
            return true;
    }

    int get_capacity(){
        return tol_capacity;
    }


private:

    data_source data_source_kind;//数据来源的种类

    algorithm algorithm_kind;//所用的算法


    int current_request;//  仅用于默认数据情况

    int tol_capacity;//总的容量

    std::list<ram_block> free_space;//空闲区域的链表

    void insert_remain(ram_block block);//按照所用的算法插入剩下的空间

    void merge_block(ram_block block);//合并挨着的空闲分区

//    bool first_fit(int lenth);//两种处理方法

//    bool best_fit(int lenth);

};

#endif // RAMMODEL_H
