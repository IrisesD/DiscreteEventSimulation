#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include <ctime>
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;
int min_event_time;
int max_event_time;
int min_next_event_time;
int max_next_event_time;
int total;
int closetime;
vector<int> money;
vector<int> event_time;
vector<int> next_event_time;
vector<int> user_number;
typedef struct node{
    int event_money;        //正数代表存款，为第二种业务，负数取款，为第一种业务
    int event_time;
    int next_event_time;
    int number;
}   *customer;
typedef struct{
    int event_type;
    int event_time;
    int num_q;
    int user_number;
}event;
bool cmp_sort(event a, event b){
    return a.event_time < b.event_time;
}
customer customer_arrive(int current_time){
    customer temp = (customer)malloc(sizeof(node));
    srand((unsigned)time(NULL));
    temp->event_money = money.front();
    money.erase(money.begin());
    temp->event_time = current_time + event_time.front();
    event_time.erase(event_time.begin());
    temp->next_event_time = current_time + next_event_time.front();
    next_event_time.erase(next_event_time.begin());
    temp->number = user_number.front();
    user_number.erase(user_number.begin());
    return temp;
}

int main() {
    int option = 1;
    json note = {};
    while (option != 0) {
        printf("请输入进行的操作:\n");
        printf("0.退出\n");
        printf("1.生成模拟数据\n");
        printf("2.生成日志文件\n");
        printf("3.生成可视化数据\n");
        printf("4.查询日志\n");
        scanf("%d", &option);
        switch (option) {
            case 0:
                break;
            case 1:
            {
                note = {};
                int current_time = 0;
                printf("请输入银行金额和营业时间:\n");
                scanf("%d%d", &total, &closetime);
                int current_money = total;
                printf("请输入用户交易时间的上下界:\n");
                scanf("%d%d", &min_event_time, &max_event_time);
                printf("请输入下一个用户到来时间的上下界:\n");
                scanf("%d%d", &min_next_event_time, &max_next_event_time);
                srand((unsigned) time(NULL));
                for (int i = 0; i < 9999; ++i) {
                    money.push_back(rand() % 1000 - 500);
                    event_time.push_back(rand() % (max_event_time - min_event_time) + min_event_time);
                    next_event_time.push_back(rand() % (max_next_event_time - min_next_event_time) + min_next_event_time);
                    user_number.push_back(i);
                }
                int num;
                printf("请输入第一种队列数量:\n");
                scanf("%d", &num);
                deque<node> q[num];
                deque<node> q0;
                deque<event> event_list;       //第一个数0表示当前用户离开，1表示下一个用户到达，第二个数存储当前事件发生的时间
                customer temp = customer_arrive(current_time);
                note[temp->number] = {};
                note[temp->number]["1"] = {};
                note[temp->number]["1"]["Event_Type"] = "Arrive";
                note[temp->number]["1"]["Event_Queue"] = 1;
                note[temp->number]["1"]["Event_Time"] = current_time;
                q[0].push_back(*temp);
                if (temp->event_money + current_money < 0) {
                    q[0].pop_front();
                    q0.push_back(*temp);
                    note[temp->number]["2"]["Event_Type"] = "Transfer to waiting queue";
                    note[temp->number]["2"]["Event_Time"] = current_time;
                }
                event a1 = *(event *) malloc(sizeof(event));        //这个用户处理业务所需时间
                a1.event_time = temp->event_time;
                a1.event_type = 0;
                a1.user_number = temp->number;
                a1.num_q = 0;
                event a2 = *(event *) malloc(sizeof(event));        //下个用户到达时间
                a2.event_time = temp->next_event_time;
                a2.event_type = 1;
                a2.user_number = temp->number +1;
                event_list.push_back(a1);
                event_list.push_back(a2);
                sort(event_list.begin(), event_list.end(), cmp_sort);
                while (!event_list.empty() && current_time < closetime) {
                    current_time = event_list.front().event_time;
                    int current_event_type = event_list.front().event_type;
                    if (current_event_type == 0) {       //当前用户离开
                        if (q[event_list.front().num_q].front().event_money + current_money < 0) {
                            note[event_list.front().user_number]["2"]["Event_Type"] = "Transfer to waiting queue";
                            note[event_list.front().user_number]["2"]["Event_Queue"] = event_list.front().num_q+1;
                            note[event_list.front().user_number]["2"]["Event_Time"] = current_time;
                            temp = &q[event_list.front().num_q].front();
                            q[event_list.front().num_q].pop_front();
                            q0.push_back(*temp);
                            event_list.pop_front();
                        }
                        else {
                            current_money += q[event_list.front().num_q].front().event_money;
                            note[event_list.front().user_number]["2"]["Event_Type"] = "Trade";
                            note[event_list.front().user_number]["2"]["Event_Time"] = current_time;
                            note[event_list.front().user_number]["2"]["Event_Queue"] = event_list.front().num_q+1;
                            note[event_list.front().user_number]["2"]["Event_Money"] = q[event_list.front().num_q].front().event_money;
                            note[event_list.front().user_number]["2"]["Current_Money"] = current_money;
                            note[event_list.front().user_number]["3"]["Event_Type"] = "Leave";
                            note[event_list.front().user_number]["3"]["Event_Time"] = current_time;
                            q[event_list.front().num_q].pop_front();
                            event_list.pop_front();
                        }
                        // 此时遍历q2,寻找可以取钱的人
                        if (q0.size() != 0) {
                            node temp = q0.front();
                            int f = 1;      //判断循环是否结束的标志
                            int len = q0.size();
                            int ll = 0;     //判断是否遍历完q2
                            while (f) {
                                while (temp.event_money + current_money < 0) {
                                    q0.pop_front();
                                    q0.push_back(temp);
                                    temp = q0.front();
                                    ll++;
                                    if (ll == len)
                                        break;
                                }
                                if (ll == len)
                                    break;
                                current_money += temp.event_money;
                                note[temp.number]["3"]["Event_Type"] = "Trade";
                                note[temp.number]["3"]["Event_Time"] = current_time;
                                note[temp.number]["3"]["Event_Money"] = current_money;
                                note[temp.number]["3"]["Event_Queue"] = 0;
                                note[temp.number]["4"]["Event_Type"] = "Leave";
                                note[temp.number]["4"]["Event_Time"] = current_time;
                                q0.pop_front();
                                ll++;
                                if (ll == len)
                                    break;
                            }
                        }
                    }
                    else{       //下一个用户到达
                        customer temp1 = customer_arrive(current_time);
                        note[temp1->number]["1"]["Event_Type"] = "Arrive";
                        note[temp1->number]["1"]["Event_Time"] = current_time;
                        int p = 0;
                        for(int k = 0; k < num; k++) {
                            if (q[k].size() < q[p].size())
                                p = k;
                        }
                        q[p].push_back(*temp1);
                        note[temp1->number]["1"]["Event_Queue"] = p+1;
                        event a1 = *(event *) malloc(sizeof(event));
                        a1.event_time = temp1->event_time;
                        a1.event_type = 0;
                        a1.user_number = temp1->number;
                        a1.num_q = p;
                        event a2 = *(event *) malloc(sizeof(event));
                        a2.event_time = temp1->next_event_time;
                        a2.event_type = 1;
                        a2.user_number = temp1->number +1;
                        if (a1.event_time < closetime)
                            event_list.push_back(a1);
                        if (a2.event_time < closetime)
                            event_list.push_back(a2);
                        event_list.pop_front();
                        sort(event_list.begin(), event_list.end(), cmp_sort);
                    }
                }
                for (int j = 0; j < num; ++j) {
                    while (!q[j].empty() || !q0.empty()){
                        if (!q[j].empty()){
                            note[q[j].front().number]["2"]["Event_Type"] = "Leave";
                            note[q[j].front().number]["2"]["Event_Time"] = closetime;
                            q[j].pop_front();
                        }
                        if (!q0.empty()){
                            note[q0.front().number]["3"]["Event_Type"] = "Leave";
                            note[q0.front().number]["3"]["Event_Time"] = closetime;
                            q0.pop_front();
                        }
                    }
                }
            }
                break;
            case 2:
            {
                cout << note.dump(4) << endl;
                fstream file_note; //可以读取txt、json等文件
                cout << "Please enter the root." << endl;
                string s;
                cin >> s;
                file_note.open(s,ios::out);
                file_note.clear();
                file_note << note.dump(4) << endl;
                file_note.close();
            }
                break;
            case 3:
            {
                system("visualization.py");
                break;
            }
            case 4:
            {
                cout << "Please enter the root." << endl;
                string s;
                cin >> s;
                fstream file_note(s);
                json note1;
                file_note >> note1;
                string s;
                string type;
                int time1,time2;
                int user1;
                int quit_flag = 0;
                while (quit_flag != 1) {
                    printf("请输入查询语句:\n");
                    cin >> s;
                    if (s != "query") {
                        printf("query error.\n");
                    }
                    else {
                        cin >> type;
                        if (type == "-u") {
                            scanf("%d", &user1);
                            cout << note1[user1].dump(4) << endl;
                        }
                        else if (type == "-t") {
                            scanf("%d%d", &time1, &time2);
                            json query;
                            json note2 = note1;
                            for (int i = 0; i < note1.size(); ++i) {
                                if (note2[i]["1"]["Event_Time"] >= time1 && note2[i]["1"]["Event_Time"] <= time2) {
                                    note2[i]["1"]["User"] = i;
                                    query[query.size()] = note2[i]["1"];
                                }
                                if (note2[i].size() >= 2  && note2[i]["2"]["Event_Time"] >= time1 && note2[i]["2"]["Event_Time"] <= time2) {
                                    note2[i]["2"]["User"] = i;
                                    query[query.size()] = note2[i]["2"];
                                }
                                if (note2[i].size() >= 3  && note2[i]["3"]["Event_Time"] >= time1 && note2[i]["3"]["Event_Time"] <= time2) {
                                    note2[i]["3"]["User"] = i;
                                    query[query.size()] = note2[i]["3"];
                                }
                                if (note2[i].size() >= 4  && note2[i]["4"]["Event_Time"] >= time1 && note2[i]["4"]["Event_Time"] <= time2) {
                                    note2[i]["4"]["User"] = i;
                                    query[query.size()] = note2[i]["4"];
                                }
                            }
                            cout << query.dump(4) << endl;
                        }
                        else if (type == "-q")
                            quit_flag = 1;
                        else if (type == "-h"){
                            cout << "Query Help" << endl;
                            cout << "1.query -h:    callout the help." << endl;
                            cout << "2.query -q:    quit the query. " << endl;
                            cout << "3.query -u a:      query the ath user's behaviors." << endl;
                            cout << "4.query -t a b:    query the behaviors during time a to b." << endl;
                            cout << "5.query -e xxx:    query the event of the type xxx" << endl;
                            cout << "6.query -s:    query the statistical data of the simulation." << endl;
                        }
                        else if (type == "-s"){
                            int number_save_person = 0;
                            int number_draw_person = 0;
                            int max_money_account = 0;
                            int min_money_account = 99999;
                            for (int i = 0; i < note1.size(); i++){
                                if (note1[i]["2"]["Event_Type"]=="Trade" && note1[i]["2"]["Event_Money"] >= 0|| note1[i]["3"]["Event_Type"]=="Trade" && note1[i]["3"]["Event_Money"] >= 0)
                                    number_save_person++;
                                else{
                                    number_draw_person++;
                                }
                                if (note1[i]["2"]["Event_Type"]=="Trade" && note1[i]["2"]["Current_Money"] >= max_money_account || note1[i]["3"]["Event_Type"]=="Trade" && note1[i]["3"]["Current_Money"] >= max_money_account)
                                    if (note1[i]["2"]["Event_Type"]=="Trade" && note1[i]["2"]["Current_Money"] >= max_money_account)
                                        max_money_account = note1[i]["2"]["Current_Money"];
                                    else
                                        max_money_account = note1[i]["3"]["Current_Money"];
                                if (note1[i]["2"]["Event_Type"]=="Trade" && note1[i]["2"]["Current_Money"] <= min_money_account || note1[i]["3"]["Event_Type"]=="Trade" && note1[i]["3"]["Current_Money"] <= min_money_account)
                                    if (note1[i]["2"]["Event_Type"]=="Trade" && note1[i]["2"]["Current_Money"] <= max_money_account)
                                        min_money_account = note1[i]["2"]["Current_Money"];
                                    else
                                        min_money_account = note1[i]["3"]["Current_Money"];
                            }

                            cout << "Draw money person:" << number_draw_person << endl;
                            cout << "Save money person:" << number_save_person << endl;
                            cout << "The max money account:" << max_money_account << endl;
                            cout << "The min money account:" << min_money_account << endl;
                        }
                        else if(type == "-e"){      //按事件类型检查
                            string event_type;
                            cin >> event_type;
                            json query_event;
                            json note3 = note1;
                            if(event_type == "Arrive") {
                                for (int i = 0; i < note1.size(); i++) {
                                    if (note1[i].size() >= 1 && note1[i]["1"]["Event_Type"] == "Arrive") {
                                        note3[i]["1"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["1"];
                                    }
                                }
                            }
                            else if(event_type == "Trade"){
                                for(int i = 0; i < note1.size();i++){
                                    if (note1[i].size() >= 2 && note1[i]["2"]["Event_Type"] == "Trade") {
                                        note3[i]["2"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["2"];
                                    }
                                    if (note1[i].size() >= 3 && note1[i]["3"]["Event_Type"] == "Trade") {
                                        note3[i]["2"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["3"];
                                    }
                                }
                            }
                            else if(event_type == "Transfer to waiting queue."){
                                for(int i = 0; i < note1.size();i++) {
                                    if (note1[i].size() >= 2 &&
                                        note1[i]["2"]["Event_Type"] == "Transfer to waiting queue."){
                                        note3[i]["2"]["User"] = i;
                                    query_event[query_event.size()] = note3[i]["2"];
                                    }
                                }
                            }
                            else if(event_type == "Leave") {
                                for (int i = 0; i < note1.size(); i++) {
                                    if (note1[i].size() >= 2 && note1[i]["2"]["Event_Type"] == "Leave"){
                                        note3[i]["2"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["2"];
                                    }
                                    if (note1[i].size() >= 3 && note1[i]["3"]["Event_Type"] == "Leave") {
                                        note3[i]["3"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["3"];
                                    }
                                    if (note1[i].size() >= 4 && note1[i]["4"]["Event_Type"] == "Leave") {
                                        note3[i]["4"]["User"] = i;
                                        query_event[query_event.size()] = note3[i]["4"];
                                    }
                                }
                            }
                            else{
                                cout << "Event type not found." << endl;
                            }
                            cout << query_event.dump(4) << endl;
                        }
                    }
                }
            }
            break;
            default:
                cout << "Command Error." << endl;
                break;
        }
    }
    return 0;
}
