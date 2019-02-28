#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 28 19:09:04 2019

@author: ariel
"""


file = open('e_example.txt','r')
file.readline()
pics = file.readlines()
hor = []
ver = []
taghor = {}
tagver = {}
for i in range(len(pics)):
    editpic = pics[i].split()
    tags = editpic[2:]
    tags.sort()
    if editpic[0]=='H':
        hor.append([i] + tags)
        for tag in tags:
            if tag in taghor:
                taghor[tag].append(i)
            else:
                taghor[tag]=[i]
    else:
        ver.append([i] + tags)
        for tag in tags:
            if tag in tagver:
                tagver[tag].append(i)
            else:
                tagver[tag]=[i]
print(hor)
print(ver)
print(taghor)
print(tagver)

