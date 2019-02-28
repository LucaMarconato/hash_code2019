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

def solve(fname):
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
    lst = []
    for i in hor:
        lst.append(i[0])
    solf = open('0' + fname, 'w')
    solf.write(str(len(lst)) + '\n')
    for i in lst:
        solf.write(str(i) + '\n')
    solf.close()
    

aplot = solve('a_example.txt')
#pp.savefig(aplot)
aplot = solve('b_example.txt')
#pp.savefig(aplot)
aplot = solve('c_example.txt')
#pp.savefig(aplot)
aplot = solve('d_example.txt')
#pp.savefig(aplot)
aplot = solve('e_example.txt')
#pp.savefig(aplot)