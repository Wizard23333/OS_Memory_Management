#include "rammodel.h"

RAMModel::RAMModel()
{
    //默认的数据来源和分配算法
    data_source_kind = DEFAULT;

    algorithm_kind = FIRST_FIT;

    current_job_num = 1;//作业号

    current_request = 0;//默认数据的请求号码

    tol_capacity = 640;

    ram_block init_block = {0, tol_capacity};
    free_space.push_back(init_block);//初始化空闲的区域
}

//bool RAMModel::apply_ram(){


//    return false;
//}

void RAMModel::merge_block(ram_block block)
{
    qDebug() << "归并前空闲区情况";
    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        qDebug() << "start_addr: " << (*it).start_addr << "lenth: " << (*it).lenth;
    }
    qDebug() << "\n";

    bool is_merged = false;//判断有没有归并过

    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        if(block.start_addr + block.lenth == (*it).start_addr)//在现有区间的左侧合并
        {
            //合并的新区间
            ram_block new_block = {block.start_addr, block.lenth + (*it).lenth};
            block = new_block;
            is_merged = true;//设置为归并过

            free_space.erase(it);//将合并的原有区间先拿出来
            break;
        }

        if(block.start_addr == (*it).start_addr + (*it).lenth)//在现有区间的右侧合并
        {
            ram_block new_block = {(*it).start_addr, block.lenth + (*it).lenth};
            block = new_block;
            is_merged = true;//设置为归并过

            free_space.erase(it);//将合并的原有区间先拿出来
            break;
        }
    }

    //如果第一次都没有归并过就直接退出
    if(is_merged == false)
    {
        qDebug() << "归并后空闲区情况";
        for(auto it = free_space.begin(); it != free_space.end(); it++)
        {
            qDebug() << "start_addr: " << (*it).start_addr << "lenth: " << (*it).lenth;
        }
        qDebug() << "\n";
        //将归并后的区插入到空闲区中
        insert_remain(block);

        return;
    }

    //第二次归并
    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        if(block.start_addr + block.lenth == (*it).start_addr)//在现有区间的左侧合并
        {
            //合并的新区间
            ram_block new_block = {block.start_addr, block.lenth + (*it).lenth};
            block = new_block;
            is_merged = true;//设置为归并过

            free_space.erase(it);//将合并的原有区间先拿出来
            break;
        }

        if(block.start_addr == (*it).start_addr + (*it).lenth)//在现有区间的右侧合并
        {
            ram_block new_block = {(*it).start_addr, block.lenth + (*it).lenth};
            block = new_block;
            is_merged = true;//设置为归并过

            free_space.erase(it);//将合并的原有区间先拿出来
            break;
        }
    }

    qDebug() << "归并后空闲区情况";
    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        qDebug() << "start_addr: " << (*it).start_addr << "lenth: " << (*it).lenth;
    }
    qDebug() << "\n";

    //将归并后的区插入到空闲区中
    insert_remain(block);
}

void RAMModel::insert_remain(ram_block block)
{
    if(free_space.empty() == true)//free_space为空的话，直接插入
    {
        free_space.push_back(block);
        return;
    }

    if(algorithm_kind == FIRST_FIT)//首次适应
    {
        for(auto it = free_space.begin(); ; it++)//按地址递增的插入
        {
            if(it == free_space.end())//在最后插入
            {
                free_space.insert(it, block);
                return;
            }

            if(block.start_addr < (*it).start_addr)//如果比当前的小，就插入
            {
                free_space.insert(it, block);
                return;
            }
        }
    }
    else if(algorithm_kind == BEST_FIT)//最佳适应
    {
        for(auto it = free_space.begin(); ; it++)\
        {
            if(it == free_space.end())//如果遍历到了最后
            {
                free_space.insert(it, block);
                return;
            }

            if(block.lenth <= (*it).lenth)//按容量递增的来插入
            {
                free_space.insert(it, block);
                return;
            }
        }
    }

    qDebug() << "error23";
}

bool RAMModel::apply_ram(int job_num, int lenth)
{
    //遍历空闲的区间，找到第一个适合区间

    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        //如果可以放下
        if((*it).lenth >= lenth)
        {
            //将内存中的作业加入到内存作业中
            job ajob = {job_num, ram_block() = {(*it).start_addr, lenth}};
            jobs_in_ram.push_back(ajob);

            //先拿出当前的空闲区间
            free_space.erase(it);
            ram_block t_block = *it;

            //如果还有剩余空间的话
            if((t_block).lenth > lenth)
            {
                //插入剩下的区间到空闲链表
                ram_block remained_block = {(t_block).start_addr + lenth, (t_block).lenth - lenth};
                insert_remain(remained_block);



//                merge_block(remained_block);
            }

            return true;
        }
    }
    return false;
}

bool RAMModel::release_ram(int job_num)
{
    for(unsigned int i = 0; i < jobs_in_ram.size(); i++)
    {
        //找到了需要释放的作业内存
        if(jobs_in_ram[i].job_num == job_num)
        {
            ram_block remained_block = jobs_in_ram[i].block_in_ram;
//            insert_remain(remained_block);//将腾出的空间继续插入到空间中
            merge_block(remained_block);//将腾出的空间合并，在merge函数中再insert

            jobs_in_ram.erase(jobs_in_ram.begin() + static_cast<int>(i));//删除对应的作业内存
            return true;
        }
    }

    return false;

}


void RAMModel::setting(data_source source, algorithm al, int capacity)
{
    data_source_kind = source;
    algorithm_kind = al;
    tol_capacity = capacity;

    //设置容量后，将空闲分区也更新
    ram_block init_block = {0, tol_capacity};
    free_space.pop_back();
    free_space.push_back(init_block);

    //每次设置的时候将序号也重置
    current_job_num = 1;
    current_request = 0;

}

request_inf RAMModel::next_step()
{
    request_inf inf;//请求的信息
    //如果是申请空间
    if(default_data[current_request][1] < 0)
    {
        //申请空间
        inf.job_num = default_data[current_request][0];
        inf.apply_or_release = -1;
        inf.request_state = apply_ram(default_data[current_request][0], abs(default_data[current_request][1]));
        inf.lenth = abs(default_data[current_request][1]);
    }
    else//释放空间
    {
        inf.job_num = default_data[current_request][0];
        inf.apply_or_release = 1;
        inf.request_state = release_ram(default_data[current_request][0]);
        inf.lenth = default_data[current_request][1];
        //inf.job_num通过引用来确定
    }

    //debug区
    qDebug() << "作业情况";
    for(unsigned int i = 0; i < jobs_in_ram.size(); i++)
    {
        qDebug() << jobs_in_ram[i].job_num << "start_addr " << jobs_in_ram[i].block_in_ram.start_addr << "lenth: " << jobs_in_ram[i].block_in_ram.lenth;
    }
    qDebug() << "\n";

    qDebug() << "空闲区情况";
    for(auto it = free_space.begin(); it != free_space.end(); it++)
    {
        qDebug() << "start_addr: " << (*it).start_addr << "lenth: " << (*it).lenth;
    }
    qDebug() << "\n";

    //如果申请成功，到下一个申请
    if(inf.request_state == true)
    {
        current_request++;
    }

    return inf;
}


RAMModel::~RAMModel()
{

}
