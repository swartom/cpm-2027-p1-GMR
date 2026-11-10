#!/usr/bin/env python3

import os
import time
data =os.listdir("./outputs")
files = []

for i in range(0,len(data),1):
    if "info" not in data[i]:
        files+= [data[i]]
# files = files[:1000]
x = []
x2 = []
y = []
y2 = []
colours = []
count = 0
outside = 0
model = dict()
model_index = 0
number = len(files)
print(number)
divergent = 0
outputfile = open("generated_file.txt","w")
outputfile2 = open("generated_file_stc.txt","w")
for f in files:
    
    count +=1
    information = []
    with open("./outputs/"+f.split(".")[0]+"_info.txt","r") as infofile:
        information = [l for l in infofile]
    size = int(information[2])
    
    if size != 256:
        print("Error with selction tool, a size different to the benchmark size has been inserted. (This is ok, and the algorithm will run if you remove this line of code, but it will not faithfully represent the resolutions represented in the paper.)")
        break
    cities = int(information[1].strip())
    if cities < 10:
        continue        
    os.system(f"./test_builder {size} ./outputs/{f}")    
    
    n = time.time()
    os.system(f"./stc_test {size} 0 data.txt")
    elapsed = time.time()-n

    
    
    file_x = []
    file_y = []
    
    for l in open("stc_output.txt"):
        file_x += [int(l.strip().split(',')[0])]
        file_y += [int(l.strip().split(',')[1])]    

    last = (-1,-1)
    last_point = -1
    turns = 0;
    non_turns = 0;

    for a,b in (zip(file_x,file_y)):                
        current_point = 0    
        if last[0] == b:
            current_point=1;
        if last[0] == a:
            current_point=1;  
        if current_point != last_point:        
            turns = turns +1;
        else:
            non_turns= non_turns +1;
        last_point = current_point;
        last = (a,b)

    for l in open("info.txt"):
        cities = int(l)           
    
    y += [(len(file_x)-1)/cities]
    
    x += [cities]
    area = information[0].split("/")[1] 
    if y[-1] > 1:
        if x[-1] > 10:
            print("example")
            print(x[-1])
            print(y[-1])
            
        outside +=1
        y = y[:-1]
        x = x[:-1]
    else:
        if area in model:            
            colours += [model[area]]
        else:
            model.update({area: model_index});
            model_index +=1
            colours += [model[area]]
        outputfile2.write(
            f"{f},{x[-1]},{y[-1]},{turns},{non_turns},{area},{colours[-1]},{elapsed}\n"#,{lkh_time},{lkh_turns},{distance},{}\n" 
        )

    n = time.time()    
    os.system(f"./test {size} 0 data.txt")    
    elapsed = time.time()-n
    c = 0
    expected_runlines = 0
    data4 = [l for l in open("info2.txt")]
    c = float(int(data4[0]))
    
    expected_runlines = float(int(data4[1]))    
    if expected_runlines == 0:
        x2 += [0]
    else: 
        x2 += [c/expected_runlines]
    
    file_x = []
    file_y = []
    
    for l in open("points_parsed.txt"):
        file_x += [int(l.strip().split(',')[0])]
        file_y += [int(l.strip().split(',')[1])]    

    last = (-1,-1)
    last_point = -1
    turns = 0;
    non_turns = 0;

    for a,b in (zip(file_x,file_y)):                
        current_point = 0    
        if last[0] == b:
            current_point=1;
        if last[0] == a:
            current_point=1;  
        if current_point != last_point:        
            turns = turns +1;
        else:
            non_turns= non_turns +1;
        last_point = current_point;
        last = (a,b)

               
    
    y2 += [(len(file_x)-1)/cities]
    
    area = information[0].split("/")[1]
     
    if y2[-1] > 1:
        print(y2[-1])
        print("how can you cover more than all of an image?")
        print(cities)
        print(f)
        exit()
    if y2[-1] > 1:
        if x[-1] > 10:
            print("example")
            
            print(y2[-1])            
        outside +=1        
    else:
        if area in model:            
            colours += [model[area]]
        else:
            model.update({area: model_index});
            model_index +=1
            colours += [model[area]]
        outputfile.write(
            f"{f},{x[-1]},{y2[-1]},{turns},{non_turns},{area},{colours[-1]},{elapsed},{x2[-1]}\n"#,{lkh_time},{lkh_turns},{distance},{x2[-1]}\n" 
        )
    if y2[-1] < y[-1]:
        print(y2[-1],y[-1])
        print(f)
        divergent +=1
        os.system(f"cp ./outputs/{f} example.txt && python mapper.py")
    if count % 1000 == 0:        
        print(f"{count} : {count/number:.2f} {f}")        


print(divergent)
