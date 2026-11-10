#!/usr/bin/env python3
#!/usr/bin/env python3
#!/usr/bin/env python3
import matplotlib.pyplot as plt


outputfile = open("generated_file.txt","r")
f = []
x = []
x2 = []
y = []
colours = []
areas = []
turns = []
time = []
non_turns = []
counter = 0
for l in outputfile:
    #f"{f},{x[-1]},{y[-1]},{turns},{non_turns},{area},{color}\n"
    data = l.split(",")
    colours += [int(data[6])]
    x2 += [float(data[-1])]
    areas += [data[5]]
    time += [data[6]]
    time += [data[6]]
    non_turns  += [int(data[4])]
    turns += [int(data[3])]
    x += [float(data[1])]
    y += [abs(float(data[2]))]
    f += [data[0]]


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
colours = [palette[i] for i in colours]

import matplotlib.pyplot as plt

plt.figure(figsize=(5,3))


import statistics as stats
count = 0
print(count)
print(len(y))
data = stats.quantiles(y,n=4)
print(data)
fig = plt.figure(figsize=(8,3))
ax = fig.subplots(1,1)
plt.sca(ax)
plt.scatter(x2,y,color="darkgray",marker="x",s=10, label="GMR")

print("Average of: ",stats.geometric_mean(y))
# plt.yscale("log")
plt.tight_layout()

outputfile = open("generated_file_stc.txt","r")
f = []
x = []
y = []
colours = []
areas = []
turns = []
time = []
non_turns = []
counter = 0
for l in outputfile:
    #f"{f},{x[-1]},{y[-1]},{turns},{non_turns},{area},{color}\n"
    data = l.split(",")
    colours += [int(data[6])]
    areas += [data[5]]
    time += [data[6]]    
    non_turns  += [int(data[4])]
    turns += [int(data[3])]
    x += [float(data[1])]
    y += [abs(float(data[2]))]
    f += [data[0]]


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
colours = [palette[i] for i in colours]

import matplotlib.pyplot as plt

plt.figure(figsize=(6,4))


import statistics as stats
count = 0
print(count)
print(len(y))
data = stats.quantiles(y,n=4)
print(data)

plt.sca(ax)
plt.scatter(x2,y,color="black",marker="o",s=10,label="STC")

plt.rcParams["pdf.use14corefonts"] = True
plt.rcParams["ps.useafm"] = True
plt.grid()
plt.xlabel("Run-Length Colourings in the Image Ratio Against Maximum in Instance")
plt.ylabel("Percentage of Optimal Tour")
plt.xscale("log")
plt.ylim([0,1.05])
print("Average of: ",stats.geometric_mean(y))
plt.legend()
plt.tight_layout()
plt.savefig("optimal_coverage_plot.pdf")
plt.savefig("optimal_coverage_plot.png")
