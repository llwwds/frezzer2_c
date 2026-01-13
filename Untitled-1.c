#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h>

int warehouse_number = 0;  // 全局变量：仓库数量，用于生成新仓库的命名编号
int frezzer_number = 0;  // 全局变量：冷冻器数量，用于生成新冰柜的命名编号
//-----------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct food {  // 食物信息
    char food_name[100];  // 食物名称，最大长度99个字符
    char food_type[100];  // 食物种类（如蔬菜、肉类、水果）
    int food_volume;  // 食物体积
    int food_temperature;  // 食物保存的温度

} food;
//-----------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct node {  // 链表节点
    food data;  // 数据域，一个food类型的变量
    struct node* next;  // 指针域，指向下一个节点
} node;

// 给node赋值
void init_node(node* n, char m_food_name[], char m_food_type[], int m_food_volume, int m_food_temperature, node* m_next) {
    strcpy(n->data.food_name, m_food_name);
    strcpy(n->data.food_type, m_food_type);
    n->data.food_volume = m_food_volume;
    n->data.food_temperature = m_food_temperature;
    n->next = m_next;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct frezzer {  // 冰柜信息
    node* head;  // 头指针
    node* tail;  // 尾指针
    int frezzer_temperature;  // 冰柜的温度
    int frezzer_available_volume;  // 冰柜的可用容积
} frezzer;

void init_frezzer(frezzer* f) {  // 给frezzer赋值
    f->head = NULL;
    f->tail = NULL;
    f->frezzer_temperature = 10;
    f->frezzer_available_volume = 100;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
// food无构造函数，node有有参数的构造函数，frezzer有有参数的构造函数
// node有析构函数，其余均无

node* create_node(node** head, node** tail) {  //新建链节并尾插，传入head和tail的二级指针
    node* temp = (node*)malloc(sizeof(node)); // 新建节点

    if (*head == NULL) {
        // 如果链表为空，head和tail指向新节点
        *head = temp;
        *tail = temp;
    } else {
        // 若链表不空，则尾插，并更新tail
        (*tail)->next = temp;
        *tail = temp;
    }

    return temp;  // 返回指向新节点的指针
}

void free_list(node* head) {  //释放整个链表，不释放头指针（在frezzer里），传入头指针
    for (node *temp = head, *next; temp != NULL; temp = next) { // 当前处理的节点
        next = temp->next;  // 保存下一个节点的地址
        free(temp);  // 释放当前节点
    }
}

int cmp(const void *a, const void *b) {  // qsort排序单链表用的排序函数
    const food *food_a = a, *food_b = b;
    return food_b->food_volume - food_a->food_volume; // 降序排序
}

void show_first_menu(){
    printf("+-----------------+-----------------+\n");
    warehouse_number=0;
    int max_warehouse_number=0;
    DIR *dir = opendir("data"); // 打开data目录，DIR为读取文件用的数据类型，若失败则返回NULL
    if (dir == NULL) {  // 若失败，则报错并返回
        printf("Error: Failed to open directory 'data'.\n");
        return;
    }
    else{  // 若成功，则遍历目录下的所有文件，将全部文件夹打印在屏幕上
        struct stat buf;
        for(struct dirent *temp=readdir(dir);temp!=NULL;temp=readdir(dir)){
            if(strcmp(temp->d_name,".")==0||strcmp(temp->d_name,"..")==0){  // 若为当前目录或上一级目录，则跳过
                continue;
            }
            else {
                char path[512];
                snprintf(path, sizeof(path), "data/%s", temp->d_name);
                if(stat(path, &buf) == 0 && S_ISDIR(buf.st_mode)){  // 若为文件夹，则打印
                    printf("| %-18s | %-18s |\n",temp->d_name,"");
                    warehouse_number++;
                }
            }
        }
        closedir(dir);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int main(){
    show_first_menu();
    return 0;
}