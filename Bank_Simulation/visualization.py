import matplotlib.pyplot as plt 
import json

x1_data = []
y1_data = []
x2_data = []
y2_data = []
queue_y = []
queue_x = [0,]
queue = []
max = 0
with open("/Users/dingcheng/Desktop/note.json") as f:       #对银行总存储金额进行可视化
    note = json.load(f)
    for cnt in range(len(note)):
        if '2' in note[cnt].keys() and 'Current_Money' in note[cnt]['2'].keys():
            y1_data.append(note[cnt]['2']['Current_Money'])
            x1_data.append(cnt)
plt.figure(1)
plt.plot(x1_data,y1_data)
plt.savefig(r'/Users/dingcheng/Desktop/Visualization/1.jpg',
                dpi=400,bbox_inches = 'tight')

with open("/Users/dingcheng/Desktop/note.json") as f:       #对用户的到达时间进行可视化
    note = json.load(f)
    for cnt in range(len(note)):
        if "Arrive" in note[cnt]['1'].values() and '2' in note[cnt].keys() and "Event_Time" in note[cnt]['2'].keys():
            y2_data.append(note[cnt]['2']['Event_Time'])
            x2_data.append(cnt)
plt.figure(2)
plt.scatter(x2_data,y2_data,s=1)
plt.savefig(r'/Users/dingcheng/Desktop/Visualization/2.jpg',
                dpi=400,bbox_inches = 'tight')


with open("/Users/dingcheng/Desktop/note.json") as f:       #对队列的长度进行可视化
    note = json.load(f)
    for cnt in range(len(note)):
        if '1' in note[cnt].keys() and "Event_Queue" in note[cnt]['1'].keys():
            if note[cnt]['1']["Event_Queue"] > max:
                max = note[cnt]['1']["Event_Queue"]
        if '2' in note[cnt].keys() and "Event_Queue" in note[cnt]['2'].keys():
            if note[cnt]['2']["Event_Queue"] > max:
                max = note[cnt]['2']["Event_Queue"]
        elif len(note[cnt]) >= 3 and "Event_Queue" in note[cnt]['3'].keys():
            if note[cnt]['3']["Event_Queue"] > max:
                max = note[cnt]['3']["Event_Queue"]
    for i in range(0,max+1):
        queue_y.append([])
    for i in range(0,max+1):
        queue_y[i].append(0)
    for cnt in range(len(note)):
        queue.append(note[cnt]["1"])
        if len(note[cnt]) >= 2 and note[cnt]['2']["Event_Type"] != "Leave":
            queue.append(note[cnt]['2'])
        if len(note[cnt]) >= 3 and note[cnt]['3']["Event_Type"] != "Leave":
            queue.append(note[cnt]['3'])
    queue = sorted(queue, key = lambda x:x["Event_Time"]);
    for i in queue:
        if i["Event_Type"] == "Arrive":
            queue_x.append(len(queue_x))
            queue_y[i["Event_Queue"]].append(queue_y[i["Event_Queue"]][-1]+1)
            for j in range(0,max+1):
                if j != i["Event_Queue"]:
                    queue_y[j].append(queue_y[j][-1])
                else:
                    pass
        elif i["Event_Type"] == "Transfer to waiting queue":
            queue_x.append(len(queue_x))
            queue_y[i["Event_Queue"]].append(queue_y[i["Event_Queue"]][-1])
            queue_y[0].append(queue_y[0][-1]+1)
            for j in range(0,max+1):
                if j != i["Event_Queue"] and j != 0:
                    queue_y[j].append(queue_y[j][-1])
        else:
            queue_x.append(len(queue_x))
            for j in range(0,max+1):
                queue_y[j].append(queue_y[j][-1])
plt.figure(3)
for i in range(0,max+1):
    plt.plot(queue_x,queue_y[i])
    plt.savefig(r'/Users/dingcheng/Desktop/Visualization/3.jpg',
                        dpi=400,bbox_inches = 'tight')

