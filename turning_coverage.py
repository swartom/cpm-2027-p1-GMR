#!/usr/bin/env python3
import matplotlib.pyplot as plt


outputfile = open("generated_file.txt","r")
outputfile2 = open("generated_file_stc.txt","r")
f = []
x = []
y = []
colours = []
areas = []
turns = []
time = []
non_turns = []
y2 = []
x2 = []
for l,t in zip(outputfile,outputfile2):
    #f"{f},{x[-1]},{y[-1]},{turns},{non_turns},{area},{color}\n"
    data = l.split(",")
    data2 = t.split(",")
    colours += [int(data[6])]
    areas += [data[5]]
    time += [data[6]]    
    non_turns  += [int(data[4])]
    turns += [int(data[3])]
    x += [float(data[1])]
    x2 += [float(data[-1])]
    y += [float(data[2])] 
    y2 += [float(data2[2])]
    y[-1] = y[-1]- y2[-1]
    f += [data[0]]
    
ls =  [(turns[i]+(x[i]-turns[i]-non_turns[i]))/(x[i]) for i in range(len(turns))]
ls = [ls[i] for i in range(len(ls)) if x[i] > 100] 

import matplotlib as mpl

sets = [list() for i in range(20)]

for i, c in zip(range(len(x)),colours):
    sets[c] += [i]    
count = 0
for a in sets:
    if a != []:
        count +=1        

colourmap = mpl.colormaps['plasma'].resampled(20)
palette = colourmap(range(count))
import numpy as np
colours = [palette[i] for i in colours]
import matplotlib.pyplot as plt

fig = plt.figure(figsize=(12,10))
print(count)
axes = fig.subfigures(count//3,3)

k = []
for a in axes:
    
    for b in a:        
        c = b.subplots(1,2,gridspec_kw={"width_ratios": [6, 1]})        
        k += [[c[0],c[1]]]
axes = k
plt.tight_layout()
for pins, ids in zip(axes,sets):    
    plt.sca(pins[0])
    plt.grid()
    plt.rcParams["pdf.use14corefonts"] = True
    plt.rcParams["ps.useafm"] = True
    plt.ylim([-0.05,1.05])    
    # plt.xlim([1,10**5])
    
    tmp_x = [(x2[i]) for i in ids ]    
    tmp_y = [(y[i]) for i in ids ]
    import numpy as np
    import statistics as stats
    plt.title(f"{areas[ids[0]]} ({len(ids)}) ({stats.geometric_mean(tmp_y)*100:.2f}%, std:{stats.stdev(tmp_y)*100:.2f}%)")
    
    col = [colours[i] for i in ids]
    plt.scatter(tmp_x,tmp_y,color=col,s=5)
    plt.sca(pins[1])
    pins[1].axis("off")
    plt.ylim([-0.05,1.05])
    bplot = plt.boxplot(tmp_y,patch_artist=True,medianprops={'color': 'black', 'linewidth': 2},showfliers=False)
    for patch, color in zip(bplot['boxes'], col[0]):
        patch.set_facecolor("darkgray")


print(len(x))
plt.savefig("coverage_comparison_plot.pdf",bbox_inches="tight")
plt.savefig("coverage_comparison_plot.png",bbox_inches="tight")
