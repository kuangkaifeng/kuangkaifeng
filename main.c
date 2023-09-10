
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"

#include <iostream>


#include <string.h>
#include <stdio.h>
#include <map>
#include <list>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define DISP_BUF_SIZE (480 * 1024)

#define PIC_FOOD_PATH "/kkf/c++/picture"


static lv_style_t style;//字库


static lv_obj_t *back_img1;//左边背景图片

static lv_obj_t *back_img2;//右边背景图片


static lv_obj_t *listbtn[50];

static lv_obj_t *btnadd[8];//相加的按钮

static lv_obj_t *btnsub[8];//相减的按钮

static lv_obj_t *img[8];//总共四张图片可放在开发板上

static lv_obj_t *img_shop;//购物订单对象

static lv_obj_t * shopmoneylabel;//购物金额标签

static lv_obj_t *shopmoneylabel2;//购物金额标签2

static lv_obj_t *shop_list;//购物列表




lv_obj_t *buybtn;//购买菜单

string addfood;//添加的食物

string cancelfood;//取消的食物

multimap<int ,string> buyfood;//购买的食物
int buyfood_num = 0;//购买的食物数量

bool flag;//加餐就是true，减餐就是false


int sockfd;

//函数的定义
void updateshoplist(string name,int price);//更新购物列表


//类的定义

#if 1
class buttn;

// 封装一个标签类
class label
{
public:
    // 构造函数
    label(lv_obj_t *parent);

    
    // 析构函数
    ~label();

    lv_obj_t* get_label_obj();

    // 设置的宽度
    void set_width(int w);
    // 设置标签高度
    void set_height(int h);
    // 设置对齐规则
    void set_align(lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);
    // 设置标签的位置
    void set_pos(lv_coord_t x, lv_coord_t y);
    // 设置标签的内容
    void set_text(const char *text);

private:
    lv_obj_t *lb;
};

class buttn
{
public:
    buttn(lv_obj_t *parent);//构造函数

    lv_obj_t *get_btn_obj();//获取对象

    ~buttn();//析构函数

    // 设置的宽度
    void set_width(int w);
    // 设置标签高度
    void set_height(int h);

    void set_event_cb(lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

    void lv_obj_align(lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);


    // void event_handler(lv_event_t * e);//事件处理函数
    
    

    



private:
    lv_obj_t *btn;
    friend class label; 
};

class food
{
    public:
        food(string name,string pathname,int price):name(name),pathname(pathname),price(price){}
    private:
        string name;
        string pathname;
        int price;
        int num=0;//食物的数量
        friend void init_foodinfo();
        friend void food_show();
        friend void show_png(map<string,list<food> > food_map,string str);
        friend void lastfood_list(lv_event_t * e);
        friend void nextfood_list(lv_event_t * e);
        friend void addmenu(lv_event_t * e);
        friend void submenu(lv_event_t * e);
        friend void updateshopmoney();
};

class shop_cart
{

    public: 
        shop_cart()
        {
            this->money=0;
        }
        void Shopping_cart()
        {
            cout<<"111111"<<endl;
            //购物车函数
            img_shop = lv_label_create(lv_scr_act());
            shopmoneylabel = lv_label_create(lv_scr_act());
            lv_obj_set_size(img_shop, 100,40);
            
            lv_obj_set_size(shopmoneylabel, 100,40);

            
            
            lv_obj_add_style(img_shop, &style, 0);
            lv_obj_add_style(shopmoneylabel, &style, 0);

            lv_label_set_text(img_shop, "总金额￥:");
            lv_label_set_text(shopmoneylabel, " ");
            lv_obj_align(img_shop, LV_ALIGN_BOTTOM_RIGHT, -200, -10);
            lv_obj_align(shopmoneylabel, LV_ALIGN_BOTTOM_RIGHT, -160, -10);
        }
        void updateadd(int price)
        {
            
            
            
            char str[100]={0};

            money+=price;
            cout<<"addprice"<<endl;
            sprintf(str," %d ",money);
            lv_label_set_text(shopmoneylabel, str);
            //lv_obj_align(shopmoneylabel, LV_ALIGN_TOP_RIGHT, 0, 40);
        }
        void updatesub(int price)
        {
            char str[100]={0};

            money-=price;
            cout<<"subprice"<<endl;
            sprintf(str," %d ",money);
            lv_label_set_text(shopmoneylabel, str);
            //lv_obj_align(shopmoneylabel, LV_ALIGN_TOP_RIGHT, 0, 40);

            
        }
    private: 
        int money;
        friend void Order(lv_event_t * e);

};

//创建一个购物车对象
static shop_cart shop_obj;

#endif


map<string,list<food> > food_map;

// list<food> current_food_list;//当前食物链表
string current_menu_name;

// 按钮事件函数，
#if 0
static void event_cb(lv_event_t *e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    // 获取当前触发事件的对象
    lv_obj_t *btn = lv_event_get_target(e);
    // 获取当前触发事件的子对象
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    // 设置标签的文本格式
    lv_label_set_text_fmt(label, "%" LV_PRIu32, cnt);
    cnt++;
}
#endif 
/**
 * Add click event to a button
 */
#if 0
void lv_mybtn(void)
{

    // 创建一个按钮对象
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    // 设置按钮的大小
    lv_obj_set_size(btn, 200, 200);
    // 把按钮放入中央
    lv_obj_center(btn);
    // lv_obj_set_pos(btn, 100, 100);

    //  添加按钮的事件 ，当按钮 被点击后就会触发  event_cb 函数
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *img;
    // 在按钮上创建可乐图片
    img = lv_img_create(btn);
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img, "S:/1.png");
    // lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_center(img);

    // 创建一个标签对象  ，在可乐图片上
    lv_obj_t *label = lv_label_create(img);
    // 设置标签的文字
    lv_label_set_text(label, "Coke 3 RMB");
    // 把标签放入按钮的中央
    lv_obj_center(label);
}
#endif

void show_backpng2(const char *pathname)
{
    back_img2 = lv_img_create(lv_scr_act());
    lv_obj_set_size(back_img2,500,420);
    /* Assuming a File system is attached to letter 'A'

     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    char cmd[1024]={0};
    sprintf(cmd,"S:%s",pathname);
    lv_img_set_src(back_img2, cmd);
    
    lv_obj_align(back_img2, LV_ALIGN_TOP_LEFT, 160, 0);//x:160-570


}

void show_backpng(const char *pathname)
{

    
    back_img1 = lv_img_create(lv_scr_act());
    lv_obj_set_size(back_img1,800,480);
    /* Assuming a File system is attached to letter 'A'

     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    char cmd[1024]={0};
    sprintf(cmd,"S:%s",pathname);
    lv_img_set_src(back_img1, cmd);
    
    lv_obj_align(back_img1, LV_ALIGN_TOP_LEFT, 0, 0);
    //lv_obj_center(img);



    // //创建下方背景
    // back_img2 = lv_img_create(lv_scr_act());
    // lv_obj_set_size(back_img2,640,80);
    // /* Assuming a File system is attached to letter 'A'

    //  * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    
    // lv_img_set_src(back_img2, cmd);
    
    // lv_obj_align(back_img2, LV_ALIGN_TOP_LEFT, 160, 400);

}

void show_buyfood()
{
    map<int,string>::iterator it=buyfood.begin();
    for(;it!=buyfood.end();it++)
    {
        cout<<"菜品"<<it->first<<"\t"<<it->second<<endl;
    }
}

bool Iscansub(string name)
{
    map<int,string>::iterator it=buyfood.begin();
    for(it;it!=buyfood.end();it++)
    {
        if(name==it->second)
        {
            

            return true;
        }
        
    }
    show_buyfood();

    return false;

}

void updatebuyfood(string name)
{
    map<int,string>::iterator it=buyfood.begin();
    for(it;it!=buyfood.end();it++)
    {
        if(name==it->second)
        {
            buyfood.erase(it);
            break;
        }
        
    }
}


// void newbuyfood()
// {
//     //遍历点单的菜单迭代器
//     multimap<int,string>::iterator it=buyfood.begin();
//     //遍历菜单
//     int num=30;
//     int i=0;
//     lv_obj_clean(buybtn);//清除之前按钮上的所有元素

//     for(it,i;it!=buyfood.end();it++,i++)
//     {
//         //有多少个元素就创建多少个标签buybtn
//         lv_obj_t *buymenulabel=lv_label_create(buybtn);

//         lv_obj_add_style(buymenulabel, &style, 0);
//         lv_label_set_text(buymenulabel,it->second.c_str());
//         lv_obj_align(buymenulabel,LV_ALIGN_TOP_MID,0,i*40);
        
//     }

// }


void canceltask(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const char *text=lv_list_get_btn_text(shop_list,obj);
    
    cout<<"撤销的内容："<<text<<endl;

    //减去金额
    map<int,string>::iterator it=buyfood.begin();
    for(it;it!=buyfood.end();it++)
    {
        if(it->second==text)
        {
            cout<<"减去的金额"<<it->first<<endl;
            shop_obj.updatesub(it->first);
            break;
        }
    }

    //删除菜单
    updatebuyfood(text);

    updateshoplist(text,0);

   
    


    

    cout<<"撤销事件"<<endl;
}

void updateshoplist(string name,int price)
{
    //
    lv_obj_clean(shop_list);
    lv_list_add_text(shop_list, "菜单");

    //更新列表
    map<int,string>::iterator it=buyfood.begin();
    int i=0;
    for(it,i;it!=buyfood.end();it++,i++)
    {
        listbtn[i]=lv_list_add_btn(shop_list, NULL, it->second.c_str());
        lv_obj_add_style(listbtn[i], &style, 0);

        lv_obj_add_event_cb(listbtn[i], canceltask, LV_EVENT_CLICKED, NULL);
    }

    // btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    // lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

}

void updateshopmoney()
{
    //先要遍历容器里面的信息找到对应的价格
    map<string,list<food> >::iterator it=food_map.begin();

    
    for(;it!=food_map.end();it++)
    {
        list<food> ::iterator it2=it->second.begin();
        for(it2;it2!=it->second.end();it2++)
        {
            if(addfood==it2->name)
            {

                shop_obj.updateadd(it2->price);


                //将点击的食物加入到订单的容器中
                buyfood.insert(make_pair(it2->price,it2->name));
                

                //测试
                show_buyfood();
                updateshoplist(it2->name,it2->price);
                //跟新点餐目录
                //newbuyfood();
            }
            if(cancelfood==it2->name)
            {
                
                if(Iscansub(it2->name))//判断添加的菜里面有没有要减的菜名
                {
                    shop_obj.updatesub(it2->price);//减金额
                    //将点击的食物从订单的容器中删除
                    updatebuyfood(it2->name);
                    updateshoplist(it2->name,it2->price);
                }
                else
                {
                    cout<<"您还没有添加要减的菜名"<<endl;
                }
                
                
                
                //更新点单容器的信息
                // updatebuyfood(it2->name);
                // show_buyfood();
                // newbuyfood();
                //跟新点餐目录
                
            }
            
        }
    }

}



void addmenu(lv_event_t * e)
{
    addfood.clear();
    cancelfood.clear();
    lv_obj_t * obj = lv_event_get_target(e);
    //找到是第几个
    lv_obj_t *p=btnadd[0];
    int i=0;
    for(i;i<sizeof(btnadd);i++)//通过btnadd按钮来判断是第几个+餐按钮，因为加餐按钮和图片按钮是一一对应的，所以找到加餐按钮的下标就是图片对象的数组下标
                                //进而推导，可以通过图片内的名字，来判断是容器内的那个菜，因此可以做出金额的相加减，  
    {
        if(obj==btnadd[i])
        {
            break;
        }
    }
    //创建一个迭代器遍历容器中的string

    
    map<string,list<food> >::iterator it=food_map.find(current_menu_name);
    list<food>  food_list=it->second;

    int j=0;

    list<food>::iterator it_food=food_list.begin();
    for(;j<i;it_food++,j++);//找到目标的前一个


    //cout<<"点餐："<<it_food->name<<endl;;

    addfood=it_food->name;

    //把点餐的信息添加到一个新的容器里面

    //更新购物车的金额
    cout<<"cancelfood:"<<cancelfood<<endl;
    cout<<"addfood:"<<addfood<<endl;

    // cancelfood.clear();

     updateshopmoney();

    

}
//前去菜单函数-----------
void submenu(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    //找到是第几个
    lv_obj_t *p=btnsub[0];
    int i=0;
    for(i;i<sizeof(btnsub);i++)//通过btnadd按钮来判断是第几个+餐按钮，因为加餐按钮和图片按钮是一一对应的，所以找到加餐按钮的下标就是图片对象的数组下标
                                //进而推导，可以通过图片内的名字，来判断是容器内的那个菜，因此可以做出金额的相加减，  
    {
        if(obj==btnsub[i])
        {
            break;
        }
    }
    //创建一个迭代器遍历容器中的string

    
    map<string,list<food> >::iterator it=food_map.find(current_menu_name);
    list<food>  food_list=it->second;

    int j=0;

    list<food>::iterator it_food=food_list.begin();
    for(;j<i;it_food++,j++);//找到目标的


    

    cancelfood=it_food->name;

    cout<<"cancelfood:"<<cancelfood<<endl;
    cout<<"addfood:"<<addfood<<endl;
    addfood.clear();

    //addlist();
    
    updateshopmoney();

}
//显示图片函数
void show_png(map<string,list<food> > food_map,string str)
{

    //每次都清除主界面
    lv_obj_clean(back_img2);
    //show_backpng2("/kkf/c++/picture/title2.png");
    map<string,list<food> >::iterator itmap=food_map.find(str);
    list<food> food1_list;

    food1_list=itmap->second;


    
    current_menu_name=itmap->first;
    cout<<"当前菜名"<<current_menu_name<<endl;
    //创建一个叠加器
    list<food>::iterator it=food1_list.begin();
    int i=0;

    //遍历链表
    for(it,i;it!=food1_list.end();it++,i++)
    {
        if(i<4)
        {
             //创建一个
            img[i] = lv_img_create(back_img2);
            
                /*Create an image button*/
            
           
            lv_img_set_src(img[i], (it->pathname). c_str());
            // lv_obj_add_style(btn, &style, 0);
            // lv_obj_add_style(btn, &style, LV_STATE_PRESSED);
            
            if(i==0)
            {

                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 0, 0);
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,200, 20);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,200, 120);


                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);

            }
            else if(i==1)
            {
                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 240, 0);
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,440, 20);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,440, 120);


                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);
            }
            else if(i==2)
            {
                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 0, 220);
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,200, 220);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,200, 320);

                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);
            }
            else if(i==3)
            {
                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 240, 220);
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,440, 220);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,440, 320);


                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);
            }


            /*Create a label on the image button*/
            lv_obj_t * label = lv_label_create(img[i]);
            lv_obj_t *label2=lv_label_create(img[i]);
            lv_obj_add_style(label, &style, 0);
            lv_obj_add_style(label2, &style, 0);
            
            lv_label_set_text(label, it->name.c_str());
            
            char str[100]={0};
            sprintf(str,"%d",it->price);

            lv_label_set_text(label2, str);
            
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(label2, LV_ALIGN_CENTER, 0, -30);
            
        }

    }


    //点击左边的任意键就显示响应的图片

    // lv_obj_t *img;
    // img = lv_img_create(lv_scr_act());
    // lv_obj_set_size(img,200,200);
    // /* Assuming a File system is attached to letter 'A'

    //  * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    // char cmd[1024]={0};
    // sprintf(cmd,"S:%s",pathname);
    // lv_img_set_src(img, cmd);
    
    // lv_obj_align(img, LV_ALIGN_TOP_LEFT, 160, 0);
    //lv_obj_center(img);


    //创建一个图片按钮


}

//初始化字库函数
void init_font()
{
    /*Create a font*/
    static lv_ft_info_t info;
    /*FreeType uses C standard file system, so no driver letter is required.*/
    info.name = "/usr/share/fonts/DroidSansFallback.ttf";
    info.weight = 15;
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    if (!lv_ft_font_init(&info))
    {
        LV_LOG_ERROR("create failed.");
    }

    /*Create style with the new font*/

    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    
}

//标签的构造函数
label::label(lv_obj_t *parent)
{
    // 创建标签对象
    lb = lv_label_create(parent);
}
// 析构函数
label::~label()
{
    // 删除标签对象
    lv_obj_del(lb);
}
// 设置的宽度
void label::set_width(int w)
{
    lv_obj_set_width(lb, w);
}
// 设置标签高度
void label::set_height(int h)
{
    lv_obj_set_height(lb, h);
}
// 设置对齐规则
void label::set_align(lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_align(lb, align, x_ofs, y_ofs);
}
// 设置标签的位置
void label::set_pos(lv_coord_t x, lv_coord_t y)
{
    lv_obj_set_pos(lb, x, y);
}
// 设置标签的内容
void label::set_text(const char *text)
{
    lv_label_set_text(lb, text);
}
//返回标签类中的成员对象
lv_obj_t* label::get_label_obj()
{
    return lb;
}

void my_test_cpp()
{
    cout << "hello LVGL" << endl;
}

//构造函数
buttn::buttn(lv_obj_t *parent)
{
    // 创建标签对象
    btn = lv_btn_create(parent);
}
//析构函数
buttn::~buttn()
{
    // 删除标签对象
    lv_obj_del(btn);
}
//设置类按钮的高度
void buttn::set_width(int w)
{
    lv_obj_set_width(btn, w);
}
//设置类按钮的宽度
void buttn::set_height(int h)
{
    lv_obj_set_height(btn, h);
}
//设置按钮的位置放置
void buttn::lv_obj_align(lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_set_style_align(btn, align, 0);
    lv_obj_set_pos(btn, x_ofs, y_ofs);
}
//获取类中按钮成员的对象
lv_obj_t * buttn::get_btn_obj()//获取对象
{
    return btn;
}

//事件函数
void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        cout<<"响应事件"<<endl;
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}
//冷菜
void Cold_Dish(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        
        cout<<"冷菜"<<endl;
    }
    //放置图片
    string str;
    str="冷菜";
    
    show_png(food_map,str);
    // lv_obj_t *img;
    // img = lv_img_create(lv_scr_act());
    // /* Assuming a File system is attached to letter 'A'

    //  * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    // lv_img_set_src(img, "");

    // // lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
    // lv_obj_center(img);


}
//招牌菜
void Signature_Dish(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        cout<<"招牌菜"<<endl;
    }   
    string str;
    str="招牌菜";
    show_png(food_map,str);
}
//热菜按钮
void Hot_Dish(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        cout<<"热菜"<<endl;
    }
    show_png(food_map,"热菜");
}
//主食
void Main_Course(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        cout<<"主食"<<endl;
    }
    show_png(food_map,"主食");
}
//酒水按钮
void Drinks(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        cout<<"酒水"<<endl;
    }
    show_png(food_map,"酒水");
}



//按钮响应
void buttn::set_event_cb(lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
}




void init_foodinfo()
{
    //检索目录
    list<food> food1_list;
    
    food1_list.push_back(food("凉拌青瓜","S:/kkf/c++/picture/凉拌青瓜.png",19));
    food1_list.push_back(food("凉拌皮蛋","S:/kkf/c++/picture/凉拌皮蛋.png",15));
    food1_list.push_back(food("凉拌土豆丝","S:/kkf/c++/picture/凉拌土豆丝.png",16));
    food1_list.push_back(food("凉拌金针菇","S:/kkf/c++/picture/凉拌金针菇.png",25));
    food1_list.push_back(food("凉拌木耳","S:/kkf/c++/picture/凉拌木耳.png",20));
    food1_list.push_back(food("凉拌鸡爪","S:/kkf/c++/picture/凉拌鸡爪.png",24));
    food_map.insert(pair<string,list<food> >("冷菜",food1_list));


    list<food> food2_list;
    food2_list.push_back(food("手撕鸡","S:/kkf/c++/picture/手撕鸡.png",47));
    food2_list.push_back(food("大盘掌翅煲","S:/kkf/c++/picture/大盘掌翅煲.png",63));
    food2_list.push_back(food("蒜香骨","S:/kkf/c++/picture/蒜香骨.png",56));
    food2_list.push_back(food("水煮牛肉","S:/kkf/c++/picture/水煮牛肉.png",53));
    food2_list.push_back(food("酸菜鱼","S:/kkf/c++/picture/酸菜鱼.png",53));
    food2_list.push_back(food("香辣虾蟹煲","S:/kkf/c++/picture/香辣虾蟹煲.png",80));
    food_map.insert(pair<string,list<food> >("招牌菜",food2_list));

    list<food> food3_list;
    food3_list.push_back(food("麻辣小龙虾","S:/kkf/c++/picture/麻辣小龙虾.png",78));
    food3_list.push_back(food("干锅牛杂","S:/kkf/c++/picture/干锅牛杂.png",45));
    food3_list.push_back(food("吴川扣肉","S:/kkf/c++/picture/吴川扣肉.png",51));
    food3_list.push_back(food("炭烧猪颈肉","S:/kkf/c++/picture/炭烧猪颈肉.png",51));
    food3_list.push_back(food("香油鸡","S:/kkf/c++/picture/香油鸡.png",47));
    food3_list.push_back(food("牛腩腐竹煲","S:/kkf/c++/picture/牛腩腐竹煲.png",45));
    food_map.insert(pair<string,list<food> >("热菜",food3_list));

    list<food> food4_list;
    food4_list.push_back(food("铁板大肠","S:/kkf/c++/picture/铁板大肠.png",48));
    food4_list.push_back(food("铁板鸡肾","S:/kkf/c++/picture/铁板鸡肾.png",40));
    food4_list.push_back(food("铁板鱿鱼","S:/kkf/c++/picture/铁板鱿鱼.png",50));
    food4_list.push_back(food("铁板牛肉","S:/kkf/c++/picture/铁板牛肉.png",60));
    food4_list.push_back(food("酸甜排骨","S:/kkf/c++/picture/酸甜排骨.png",53));
    food4_list.push_back(food("青椒炒猪肝","S:/kkf/c++/picture/青椒炒猪肝.png",43));
    food_map.insert(pair<string,list<food> >("主食",food4_list));


    list<food> food5_list;
    food5_list.push_back(food("可乐","S:/kkf/c++/picture/可乐.png",5));
    food5_list.push_back(food("美年达","S:/kkf/c++/picture/美年达.png",5));
    food5_list.push_back(food("加多宝","S:/kkf/c++/picture/加多宝.png",6));
    food5_list.push_back(food("百威","S:/kkf/c++/picture/百威.png",9));
    food5_list.push_back(food("乳酸菌","S:/kkf/c++/picture/乳酸菌.png",8));
    food5_list.push_back(food("雪碧","S:/kkf/c++/picture/雪碧.png",4));
    food_map.insert(pair<string,list<food> >("酒水",food5_list));





}

void food_show()
{
        //测试打印map容器的标签

    map<string,list<food> >::iterator it;

    for(it=food_map.begin();it!=food_map.end();it++)
    {
        cout<<"food_map容器标签为："<<it->first<<endl;
        list<food>::iterator it2;
        for(it2=it->second.begin();it2!=it->second.end();it2++)
        {
            cout<<"食物的名称:"<<it2->name<<"图片的路径:"<<it2->pathname<<"价格:"<<it2->price<<endl;
        }
    }
}


void lastfood_list(lv_event_t * e)
{
    show_png(food_map,current_menu_name);
    
}

void nextfood_list(lv_event_t * e)
{
    lv_obj_clean(back_img2);

    //show_backpng2("/kkf/c++/picture/title2.png");
    cout<<"当前菜单名:"<<current_menu_name<<endl;

    map<string,list<food> >::iterator itmap=food_map.find(current_menu_name);
    

    list<food> ::iterator it=itmap->second.begin();
    




    int i=0;

     //遍历链表
    for(it,i;it!=itmap->second.end();it++,i++)
    {


        if(i>3)
        {
            //创建一个
            img[i] = lv_img_create(back_img2);
                /*Create an image button*/

            
            
            lv_img_set_src(img[i], (it->pathname). c_str());
            if(i==4)
            {
                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 0, 0);
                //创建按钮
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,200, 20);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,200, 120);

                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);


                cout<<"pathname:"<<it->pathname<<endl;
            }
            else if(i==5)
            {
                lv_obj_align(img[i], LV_ALIGN_TOP_LEFT, 240, 0);
                //创建按钮
                btnadd[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnadd[i], addmenu, LV_EVENT_CLICKED,NULL);
                btnsub[i]=lv_btn_create(back_img2);
                lv_obj_add_event_cb(btnsub[i], submenu, LV_EVENT_CLICKED,NULL);
                lv_obj_set_size(btnadd[i], 40, 80);
                lv_obj_set_size(btnsub[i], 40, 80);
                lv_obj_align(btnadd[i], LV_ALIGN_TOP_LEFT,440, 20);
                lv_obj_align(btnsub[i], LV_ALIGN_TOP_LEFT,440, 120);


                //在按钮上创建标签
                lv_obj_t *labeladd = lv_label_create(btnadd[i]);
                lv_obj_t *labelsub=lv_label_create(btnsub[i]);
                lv_label_set_text(labeladd, "+");
                lv_label_set_text(labelsub, "-");
                lv_obj_center(labeladd);
                lv_obj_center(labelsub);


                cout<<"pathname:"<<it->pathname<<endl;
            }

            lv_obj_t * label = lv_label_create(img[i]);
            lv_obj_t *label2=lv_label_create(img[i]);
            lv_obj_add_style(label, &style, 0);
            lv_obj_add_style(label2, &style, 0);
            
            lv_label_set_text(label, it->name.c_str());
            
            char str[100]={0};
            sprintf(str,"%d",it->price);

            lv_label_set_text(label2, str);
            
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(label2, LV_ALIGN_CENTER, 0, -30);

        }

    } 


}



void Order(lv_event_t * e)
{
    cout<<"下单成功"<<endl;
    //创建套接字文件
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        cout<<"创建套接字失败"<<endl;
    }
    cout<<"创建套接字成功"<<endl;


    //绑定套接字文件
    struct sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8080);
    servaddr.sin_addr.s_addr=inet_addr("192.168.60.31");
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
    {
        cout<<"连接服务器失败"<<endl;
    }
    cout<<"连接服务器成功"<<endl;


    //发送数据
    char buf[1024]={0};
    
    multimap<int,string>::iterator it=buyfood.begin();
    for(it;it!=buyfood.end();it++)
    {
        sprintf(buf,"%s\r\n%s,%d",buf,it->second.c_str(),it->first);
    }
    
    sprintf(buf,"%s\r\n总金额:%d\r\n",buf,shop_obj.money);
    cout<<"buf:"<<buf<<endl;
    write(sockfd,buf,sizeof(buf));

    close(sockfd);

    //下单成功之后就需要清空菜单购物车
    buyfood.clear();
    cout<<"清空容器信息完成"<<endl;
    //更新购物车里面的信息
    lv_obj_clean(shop_list);
    lv_list_add_text(shop_list, "菜单");
    lv_label_set_text(shopmoneylabel, " ");

    cout<<"清空购物车完成"<<endl;
    shop_obj.money=0;
    shop_obj.Shopping_cart();
    //金额清空
    cout<<"金额清空"<<endl;


}




void Main_Interface()
{
    //将食谱的照片分别存入到map容器中
    init_foodinfo();

    food_show();

    

    //创建背景
    show_backpng("/kkf/c++/picture/title1.png");
    show_backpng2("/kkf/c++/picture/title2.png");
    //show_png("/kkf/c++/picture/凉拌皮蛋.png");
    //创建按钮类
    buttn *btn = new buttn(lv_scr_act());
    btn->set_width(150);
    btn->set_height(50);
    btn->set_event_cb(Cold_Dish, LV_EVENT_CLICKED, NULL);
    btn->lv_obj_align(LV_ALIGN_TOP_LEFT, 10, 10);
    //创建标签对象
    label *lb=new label(btn->get_btn_obj());
    lv_obj_add_style(lb->get_label_obj(), &style, 0);
    lb->set_text("冷菜");
    lv_obj_center(lb->get_label_obj());


    //创建按钮类
    buttn *btn1 = new buttn(lv_scr_act());
    btn1->set_width(150);
    btn1->set_height(50);
    btn1->set_event_cb(Signature_Dish, LV_EVENT_CLICKED, NULL);
    btn1->lv_obj_align(LV_ALIGN_TOP_LEFT, 10, 80);
    //创建标签对象
    label *lb1=new label(btn1->get_btn_obj());
    lv_obj_add_style(lb1->get_label_obj(), &style, 0);
    lb1->set_text("招牌菜");
    lv_obj_center(lb1->get_label_obj());


    buttn *btn2 = new buttn(lv_scr_act());
    btn2->set_width(150);
    btn2->set_height(50);
    btn2->set_event_cb(Hot_Dish, LV_EVENT_CLICKED, NULL);
    btn2->lv_obj_align(LV_ALIGN_TOP_LEFT, 10, 150);
    //创建标签对象
    label *lb2=new label(btn2->get_btn_obj());
    lv_obj_add_style(lb2->get_label_obj(), &style, 0);
    lb2->set_text("热菜");
    lv_obj_center(lb2->get_label_obj());


    buttn *btn3 = new buttn(lv_scr_act());
    btn3->set_width(150);
    btn3->set_height(50);
    btn3->set_event_cb(Main_Course, LV_EVENT_CLICKED, NULL);
    btn3->lv_obj_align(LV_ALIGN_TOP_LEFT, 10, 220);
    //创建标签对象
    label *lb3=new label(btn3->get_btn_obj());
    lv_obj_add_style(lb3->get_label_obj(), &style, 0);
    lb3->set_text("主食");
    lv_obj_center(lb3->get_label_obj());


    buttn *btn4 = new buttn(lv_scr_act());
    btn4->set_width(150);
    btn4->set_height(50);
    btn4->set_event_cb(Drinks, LV_EVENT_CLICKED, NULL);
    btn4->lv_obj_align(LV_ALIGN_TOP_LEFT, 10, 290);
    //创建标签对象
    label *lb4=new label(btn4->get_btn_obj());
    lv_obj_add_style(lb4->get_label_obj(), &style, 0);
    lb4->set_text("酒水");
    lv_obj_center(lb4->get_label_obj());





    buttn *btn5 = new buttn(lv_scr_act());
    btn5->set_width(100);
    btn5->set_height(50);
    btn5->set_event_cb(lastfood_list, LV_EVENT_CLICKED, NULL);
    btn5->lv_obj_align(LV_ALIGN_BOTTOM_MID, -250, 0);
    //创建标签对象
    label *lb5=new label(btn5->get_btn_obj());
    lb5->set_text("<<<");
    lv_obj_center(lb5->get_label_obj());



    buttn *btn6 = new buttn(lv_scr_act());
    btn6->set_width(100);
    btn6->set_height(50);
    btn6->set_event_cb(nextfood_list, LV_EVENT_CLICKED, NULL);
    btn6->lv_obj_align(LV_ALIGN_BOTTOM_MID, 50, 0);
    //创建标签对象
    label *lb6=new label(btn6->get_btn_obj());
    lb6->set_text(">>>");
    lv_obj_center(lb6->get_label_obj());

    
    shop_obj.Shopping_cart();//创建购物车方块
    
    
    //创建购物车列表
    shop_list = lv_list_create(lv_scr_act());
    lv_obj_set_size(shop_list, 140, 380);
    lv_obj_align(shop_list, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(shop_list,&style,0);
    lv_list_add_text(shop_list, "菜单");




    




    buttn *btn7 = new buttn(lv_scr_act());
    btn7->set_width(140);
    btn7->set_height(100);
    btn7->set_event_cb(Order, LV_EVENT_CLICKED, NULL);
    btn7->lv_obj_align(LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    //创建标签对象
    label *lb7=new label(btn7->get_btn_obj());
    lv_obj_add_style(lb7->get_label_obj(), &style, 0);
    lb7->set_text("下单");
    lv_obj_center(lb7->get_label_obj());


    // buybtn=lv_btn_create(lv_scr_act());
    // lv_obj_set_size(buybtn, 140, 280);
    // lv_obj_align(buybtn, LV_ALIGN_TOP_RIGHT, 0, 100);








}


int main(void)
{
    /*lvgl初始化*/
    lv_init();

    /*输出设备初始化及注册*/
    fbdev_init();
    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    lv_disp_drv_register(&disp_drv);

    // 输入设备初始化及注册
    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

    // 官方demo---可以换为自己的demo
    //  lv_demo_widgets();
    //   lv_example_btn_1();

    // 运行官方的事件例子
    // lv_example_event_1();
    // lv_example_imgbtn_my();
    // lv_example_freetype_1();
    // show_font(300, 50, "我爱你,爱着你就像老鼠爱大米.");
    // show_font(300, 150, "就算有多少风雨我依然爱着你.");
    // show_font(300, 250, "我想你,想着你的.");

    // base tmp(11086);
    // tmp.show();

    init_font();
    Main_Interface();

    /*事物处理及告知lvgl节拍数*/
    while (1)
    {
        lv_timer_handler(); // 事务处理
        lv_tick_inc(5);     // 节拍累计
        usleep(5000);
    }

    return 0;
}

/*用户节拍获取*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0)
    {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
