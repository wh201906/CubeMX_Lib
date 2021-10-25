from shutil import copyfile
from re import sub, escape
import os

currID = None


def newDefine(matched):
    global currID
    string = matched.group(0)
    if string[-1] == ',':
        string = string.rstrip(1)
    string += ',NODE_ID=' + hex(currID + ord('0'))
    return string


name = 'F407VE_8M_WaveTransfer'
suffix = '.uvprojx'
pattern = r'(?<=' + escape('<Define>') + ').+(?=' + escape('</Define>') + ')'
IDList = [0, 1]

print()
for id in IDList:
    currID = id
    newFile = os.path.join(os.path.dirname(__file__), name + '_' + str(id) + suffix)
    copyfile(os.path.join(os.path.dirname(__file__), name + suffix), newFile)
    with open(newFile, mode='r+') as f:
        data = f.read()
        data = sub(pattern, newDefine, data)
        f.seek(0)
        f.truncate()
        f.write(data)
