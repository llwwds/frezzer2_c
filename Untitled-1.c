#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h>

int warehouse_number = 0;  // 全局变量：仓库数量，用于生成新仓库的命名编号
int frezzer_number = 0;  // 全局变量：冰柜数量，用于生成新冰柜的命名编号

enum menu{firest,second,third};  // 定义枚举类型，用于记录现在要显示哪个界面
enum menu menu_state=firest;  // 记录当前页面，刚进入程序默认为一级菜单
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

void init_frezzer(frezzer* f) {  // 给frezzer赋初值
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
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int cmp(const void *a, const void *b) {  // qsort排序单链表用的排序函数
    const food *food_a = a, *food_b = b;
    return food_b->food_volume - food_a->food_volume; // 降序排序
}

void sort_food_list(frezzer* f) {  //对冰柜中的食物按照体积进行 降序排序 传入指向冰柜变量的指针
    if (f->head == NULL || f->head->next == NULL) return; // 若为空链表/单节点，不用排序

    food temp_data[100];  // 暂时记录链表中全部数据
    int tag = 0;  // 临时变量，用两次

    // 1. 复制链表数据到数组（无初始化）
    for (node* temp = f->head; temp != NULL; temp = temp->next) {
        temp_data[tag++] = temp->data;
    }

    // 2. 用实际节点数 tag 替代固定 100
    qsort(temp_data, tag, sizeof(food), cmp);

    // 3. 复制回链表
    tag = 0;
    for (node* temp = f->head; temp != NULL; temp = temp->next) {
        temp->data = temp_data[tag++];
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void show_first_menu(){  // 显示一级菜单(仓库们)
    printf("======================================\n");
    printf("           [ First menu ]             \n");
    printf("+-----------------+-----------------+1\n");
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
            if(strcmp(temp->d_name,".")==0||strcmp(temp->d_name,"..")==0){  // 若文件名显示为当前目录或上一级目录，则跳过
                continue;
            }
            else {
                char path[512];  // 规定拼路径时的缓冲区长度
                snprintf(path, sizeof(path), "data/%s", temp->d_name);  // 用temp指向的文件的名字，拼出完整的文件相对路径
                if(stat(path, &buf) == 0 && S_ISDIR(buf.st_mode)){  // 若为目标路径位置的文件为文件夹，则打印
                    printf(" %-18s\n",temp->d_name,"");
                    warehouse_number = (warehouse_number > atoi(temp->d_name+9)) ? warehouse_number : atoi(temp->d_name+9);  // 保证仓库编号计数器始终是所有仓库中编号最大的+1
                    // atoi可以将字符串转为数字，d_name+9是因为本质上字符串是数组，字符串名是指针，指针+9就是访问第9位
                }
            }
        }
        closedir(dir);
        warehouse_number++;
    }
    printf("+-----------------+-----------------+1\n");
    printf("\n");
    printf("Enter -1 to exit\n");
    printf("Enter 0  to open a warehouse\n");
    printf("Enter 1  to create a new warehouse\n");
    printf("Enter 2  to delete a warehouse\n");
    printf("\n");
    printf("======================================\n");
}

void show_second_menu(char file_path[]){  // 显示二级菜单(仓库内的冰柜们)，传入拼好的文件路径（到仓库，不到每一个冰柜）
    frezzer_number=0;  // 冰柜数量计数器清零，接下来重新计数
    DIR *dir = opendir(file_path);  // 打开文件夹，新建一个文件夹指针dir指向打开的文件夹用于后续操作，若失败则返回NULL
    if(dir==NULL){
        printf("Error: Failed to open directory %s.\n",file_path);
        menu_state=firest;  // 把”当前菜单“改回一级菜单
        return;
    }
    printf("======================================\n");
    printf("          [ Second menu ]             \n");
    printf("+-----------------+-----------------+2\n");
        for(struct dirent *temp=readdir(dir);temp!=NULL;temp=readdir(dir)){  // 若能打开，则开始遍历文件夹内，查找txt文件
            if(strcmp(temp->d_name,".")==0||strcmp(temp->d_name,"..")==0){  // 若文件名显示为当前目录或上一级目录，则跳过
                continue;
            }
            else {
                char path[512];  // 规定拼路径时的缓冲区长度
                struct stat buf;
                snprintf(path, sizeof(path), "%s/%s", file_path, temp->d_name);  // 用temp指向的文件的名字，拼出完整的文件相对路径
                if(stat(path, &buf) == 0 && strlen(path) >= 4 && strncmp(path + strlen(path) - 4, ".txt", 4) == 0){  // 若为目标路径位置的文件为txt，则打印
                    printf(" %-18s\n",temp->d_name);
                    frezzer_number = (frezzer_number > atoi(temp->d_name+7)) ? frezzer_number : atoi(temp->d_name+7);  // 保证仓库编号计数器始终是所有仓库中编号最大的+1
                }
            }
            frezzer_number++;
        
    }
    closedir(dir);
    printf("+-----------------+-----------------+2\n");
    printf("\n");
    printf("Enter -1 to back to the first menu\n");
    printf("Enter 0  to open a frezzer\n");
    printf("Enter 1  to create a new frezzer\n");
    printf("Enter 2  to delete a frezzer\n");
    printf("\n");
    printf("======================================\n");
    return ;
}

void show_third_menu(char file_path[]){  // 显示三级菜单(冰柜内的食物们)，传入拼好的文件路径（到每一个冰柜）
    FILE *file=fopen(file_path,"r");  // 以读模式打开文件路径，新建一个文件指针file指向打开的文件用于后续操作，若失败则返回NULL
    if(file==NULL){
        printf("Error: Failed to open file %s.\n",file_path);
        menu_state=second;  // 把”当前菜单“改回二级菜单
        return;
    }
    printf("======================================\n");
    printf("           [ Third menu ]             \n");
    printf("+-----------------+-----------------+3\n");
    
        frezzer f;
        init_frezzer(&f);  // 初始化frezzer
        int lowest_temperature=10;
        int used_volume=0;
        
        char temp_name[100];  // 新建四个变量，用于临时存放读到的四项数据
        char temp_type[100];
        int temp_vol;
        int temp_temp;
        // txt文件内顺序：名称 种类 体积 温度
        while(fscanf(file, "%s %s %d %d", temp_name, temp_type, &temp_vol, &temp_temp) == 4){  // 从文件中读取数据，直到读完，读的内容存入4个临时变量中
            node* temp = create_node(&(f.head), &(f.tail));  // 新建节点并尾插
            init_node(temp, temp_name, temp_type, temp_vol, temp_temp, NULL); // 初始化节点，将临时变量中的数据存入节点中

                if(lowest_temperature > temp->data.food_temperature){  // 计算最低温度
                    lowest_temperature = temp->data.food_temperature;
                }
                used_volume += temp->data.food_volume;  // 计算已用体积
            }
        sort_food_list(&f);  // 对冰柜中的食物按照体积进行 降序排序
        f.frezzer_available_volume-=used_volume;  // 更新可用体积
        f.frezzer_temperature=lowest_temperature;  // 更新最低温度

        for(node*temp=f.head;temp!=NULL;temp=temp->next){  // 遍历链表，打印每个节点的数据
            printf("| %-18s | %-18s | %-18d | %-18d |\n",temp->data.food_name,temp->data.food_type,temp->data.food_volume,temp->data.food_temperature);
        }
    
    fclose(file);
    printf("+-----------------+-----------------+3\n");
    printf("\n");
    printf("Enter -1 to back to the second menu\n");
    printf("Enter 0  to subscribe llwwds on github\n");
    printf("Enter 1  to add a new food\n");
    printf("Enter 2  to delete a food\n");
    printf("\n");
    printf("======================================\n");
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(){
    
    int choice = 0;  // 记录用户输入的选项
    // -1退出 0打开 1新建 2删除 ，操作时输入编号然后拼出路径进行文件操作

    char target_warehouse_path[600] = "";  // 记录当前选中的仓库路径,例："data/warehouse1"
    char target_freezer_path[600] = "";  // 记录当前选中的冰柜文件路径，例："data/warehouse1/frezzer1.txt"
    
    while(1){
        if(menu_state==third){show_third_menu(target_freezer_path);}  // 先显示菜单界面，等用户输入指令
        else if(menu_state==second){show_second_menu(target_warehouse_path);}
        else {show_first_menu();}  // 遇事不决，显示主菜单,不能出了问题就什么都不显示

        scanf("%d",&choice);

        if(menu_state==firest){  // 如果当前在一级菜单

            if(choice==-1){  // 一级菜单-退出程序
                break;
            }

            else if(choice==0){  // 打开二级菜单
                printf("Please enter the number");
                int number;  // 承接输入的编号，稍后用于拼接
                scanf("%d",&number);
                sprintf(target_warehouse_path, "data/warehouse%d", number);  // 拼出目标文件夹的路径
                menu_state=second;
            }
            else if(choice==1){  // 新建一个文件夹
                char temp_path[600];
                sprintf(temp_path, "data/warehouse%d", warehouse_number);  // 拼出目标文件夹的路径
                int temp = mkdir(temp_path);
                if(temp==0){
                    printf("Done\n");
                }
                else{
                    printf("Failed to create a new warehouse\n");
                }
            }
            else if(choice==2){  // 删除一个文件夹
                printf("Please enter the number");
                int number;  // 承接输入的编号，稍后用于拼接
                scanf("%d",&number);
                char temp_path[600];  // 临时承接要删除的文件夹的路径
                sprintf(temp_path, "data/warehouse%d", number);  // 拼出目标文件夹的路径

                struct stat st;
                if(stat(temp_path,&st)==0&&S_ISDIR(st.st_mode)){  // 检查目标路径位置是否打开成功，若是则判断是不是一个文件夹，全满足才下一步
                    DIR *dir=opendir(temp_path);  // rmdir只能删除空文件夹，故只能打开文件夹删除里面的所有文件然后再删文件夹
                    if(dir==NULL){
                        printf("The warehouse is not exist\n");
                        continue;
                    }
                    for(struct dirent *temp=readdir(dir);temp!=NULL;temp=readdir(dir)){
                        if(strcmp(temp->d_name,".")==0||strcmp(temp->d_name,"..")==0){
                            continue;
                        }
                        else{
                            char temp_frezzer_path[600];  // 接下来要删除的frezzer的文件路径
                            snprintf(temp_frezzer_path, sizeof(temp_frezzer_path), "%s/%s", temp_path, temp->d_name);  // 拼出文件夹中的一个frezzer的路径，接下来删除它
                            remove(temp_frezzer_path);
                        }
                    }
                    closedir(dir);
                    rmdir(temp_path);
                }
                else{
                    printf("The warehouse is not exist\n");
                }
                
            }
            else{
                printf("he yi wei ?");  // 若输入非法内容，则报错
            }
        }

        else if(menu_state==second){  // 如果当前在二级菜单
            if(choice==-1){  // 返回一级菜单
                menu_state=firest;
                sprintf(target_warehouse_path, "");
            }
            else if(choice==0){  // 打开一个冰柜
                printf("Please enter the number");
                int number;  // 承接输入的编号，稍后用于拼接
                scanf("%d",&number);
                sprintf(target_freezer_path, "%s/frezzer%d.txt", target_warehouse_path, number);  // 拼出目标文件夹的路径
                menu_state=third;
            }
            else if(choice==1){  // 新建一个冰柜
                char temp_path[600];
                sprintf(temp_path, "%s/frezzer%d.txt", target_warehouse_path, frezzer_number);  // 拼出目标文件夹的路径
                FILE *file = fopen(temp_path, "w");
                if(file==NULL){
                    printf("Failed to create the frezzer\n");
                }
                else{
                    fclose(file);
                    printf("Done\n");
                }
            }
            else if(choice==2){  // 删除一个冰柜
                
            }
            else{
                printf("he yi wei ?");  // 若输入非法内容，则报错
            }
        }
        
        else if(menu_state==third){  // 如果当前在三级菜单
            if(choice==-1){  // 返回二级菜单
                menu_state=second;
                sprintf(target_freezer_path, "");
            }
            else if(choice==0){}
            else if(choice==1){}
            else if(choice==2){}
            else{
                printf("he yi wei ?");  // 若输入非法内容，则报错
            }
        }
    }
    return 0;
}