#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 28 19:09:04 2019

@author: ariel
"""

import matplotlib.pyplot as plt
from collections import Counter
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.pyplot import figure
figure(num=None, figsize=(16, 9), dpi=80, facecolor='w', edgecolor='k')

def dotheplots(fname):
    file = open(fname,'r')
    file.readline()
    pics = file.readlines()
    file.close()
    print("Reading done")
    hor = []
    ver = []
    taghor = {}
    tagver = {}
    taghorlen = {}
    tagverlen = {}
    for i in range(len(pics)):
        editpic = pics[i].split()
        tags = editpic[2:]
        tags.sort()
        if editpic[0]=='H':
            hor.append([i] + tags)
            for tag in tags:
                if tag in taghor:
                    taghor[tag].append(i)
                    taghorlen[tag] += 1
                else:
                    taghor[tag]=[i]
                    taghorlen[tag] = 1
        else:
            ver.append([i] + tags)
            for tag in tags:
                if tag in tagver:
                    tagver[tag].append(i)
                    tagverlen[tag] += 1
                else:
                    tagver[tag]=[i]
                    tagverlen[tag] = 1
    tagdict = dict(Counter(taghorlen)+ Counter(tagverlen))
    print("Computation done")                
    def plotdict(dic):
        value = list(dic.values())
        value.sort(reverse= True)
        return plt.plot(value)
    def plotlist(ls):
        lst = [len(i) for i in ls]
        lst.sort(reverse=True)
        return plt.plot(lst)
    grid = plt.GridSpec(3, 3, wspace=0.5, hspace=0.5)
    plt.subplot(grid[0:2,:])
    plt.title(fname[0] + " Tags")
    p1 = plotdict(taghorlen)
    p2 = plotdict(tagverlen)
    p3 = plotdict(tagdict)
    plt.ylabel("Count")
    plt.xlabel("Tags")
    plt.legend((p1[0], p2[0],p3[0]), ('Horizontal', 'Vertical','Total'))
    plt.subplot(grid[2,0])
    plt.title(fname[0] + " Total Tag per Photo")
    plt.ylabel("# Tags")
    plt.xlabel("Total Slides")
    p1 = plotlist(hor+ver)
    plt.subplot(grid[2,1])
    plt.title(fname[0] + " Horizontal Tag per Photo")
    plt.ylabel("# Tags")
    plt.xlabel("Horizontal Slides")
    p1 = plotlist(hor)
    plt.subplot(grid[2,2])
    plt.title(fname[0] + " Vertical Tag per Photo")
    plt.ylabel("# Tags")
    plt.xlabel("Vertical Slides")
    p1 = plotlist(ver)
    pp.savefig()

pp = PdfPages('Eda.pdf')
aplot = dotheplots('a_example.txt')
#pp.savefig(aplot)
aplot = dotheplots('b_example.txt')
#pp.savefig(aplot)
aplot = dotheplots('c_example.txt')
#pp.savefig(aplot)
aplot = dotheplots('d_example.txt')
#pp.savefig(aplot)
aplot = dotheplots('e_example.txt')
#pp.savefig(aplot)
pp.close()