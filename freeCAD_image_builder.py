#!/usr/bin/env python3

layer_count = 10;
size = 256;

import os
import trimesh
import numpy as np
import matplotlib.pyplot as plt

os.system("rm list.txt")
os.system("find . -name *.stl >> list.txt")
os.system("mkdir layers")
os.system("mkdir outputs")


length = len([l for l in open("list.txt")])
counter = 0

for l in open("list.txt"):
    counter +=1
    print(f"{counter/length*100:.2f}" ,": "+ l)    
    file_name = l.strip();
    filename = f"layers{"".join(l.strip().split('.')[:-1]).replace(" ","_").replace("&","_").replace("(","_").replace(")","_")}/"
    data = trimesh.load(file_name)    
    os.system(f"mkdir -p {filename}")
    bordering =0
    try:
        h_min, h_max = data.bounds[:,2]
        layers = (h_max - h_min)/layer_count;
        lvls = np.arange(h_min, h_max,layers)
        sections = data.section_multiplane( plane_origin=data.bounds[0], plane_normal=[0, 0, 1], heights=lvls)
        i = 0
    except Exception:
        print("None Type Error")
    
    for planar in sections:
        plt.close("all")
        plt.cla()
        plt.figure(figsize=(5,5))
        
        maxx = 0
        minx = 100000
        marker = True
        if planar == None:
            continue
        try:
            for p in planar.polygons_full:        
                x,y = p.exterior.xy        
                plt.fill(x,y,color="black")
            for h in p.interiors:
                x,y = h.xy            
                plt.fill(x,y,color="white")
        except Exception as e:
            marker = False
            print("Error",e)
        finally:
            plt.tight_layout()
        if not marker:
            continue
        plt.axis("off")
        i+=1
        im_file = filename+f"layer_{i}_mesh.png"
        plt.savefig(im_file)        
        import cv2
        import os                
        import numpy as np
        im =cv2.imread(im_file)
        
        im = cv2.resize(im,[size,size])
        mask = cv2.inRange(im,np.array([0,0,0]),np.array([0,0,0] ))        
        nb_blobs, lbls, _, _ = cv2.connectedComponentsWithStats(mask)        
        for j in range(1,nb_blobs):
            comp = np.ones([size,size])
            comp[lbls == j] = 0;
            plt.close("all")
            # plt.imshow(comp)
            # plt.show()
                            
            available = 0
            vals = []
            with open(f"outputs/size_{size}_{counter}_{i}_{j}.txt","w") as f:        
                for k in comp:            
                    for a in k:                
                        if a == 0:
                            f.write(" ")
                            available +=1
                        else:                            
                            f.write("x")
                    f.write("\n")
            with open(f"outputs/size_{size}_{counter}_{i}_{j}_info.txt","w") as f:
                f.write(filename + "\n")                
                f.write(f"{available}\n{size}")                
                
            
