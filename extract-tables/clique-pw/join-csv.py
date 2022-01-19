import json
import sys
import os
import argparse
import re
import csv
import itertools
from shutil import move
from tempfile import NamedTemporaryFile


parser = argparse.ArgumentParser(description='|--| Extract information of ALL jobs to given .csv file |--|')
parser.add_argument('--files', nargs='+', type=argparse.FileType(), help='Files that need to get processed')
#parser.add_argument('--key', nargs='+', required=True, help='Common headers in csv files')
args = parser.parse_args()

#print(args.files)
print("----------------------------------------------------\n")
#print(args.key)

headers = {}
fields = []
for file in args.files:
    if not os.path.splitext(file.name)[1] in [".csv"]:
            print(file.name + " has invalid extension")
            quit()
# Read headers into a list of lists
for file in args.files:
    f = open(file.name)
    r = csv.DictReader(f)
    headers[file.name] = (r.fieldnames)
    fields.extend(r.fieldnames)

# check that columns are disjoint
result_list = map(dict, itertools.combinations(headers.items(), 2))
for item in result_list:
    values = item.values()
    values_list = list(values)
    common_headers = list(set(values_list[0])&set(values_list[1]))
    # if len(common_headers) != len(args.key):
    #     for key in item.keys():
    #         print(key)
    #     for h in common_headers:
    #         print(h+" ", end="")
    #     print(common_headers)
    #     print("Two files have invalid intersection of headers")
    #     quit()
    # for key in args.key:
    #     if not key in common_headers:
    #         print(str(item))
    #         print(key +" is not in the printed headers")
    #         quit()


#

fields = list(set(fields))
initialRow = {}
for key in fields:
    initialRow[key] = "NaN"

keysList = []
rowsDictionary = {}


for file in args.files:
     f = open(file.name)
     r = csv.DictReader(f)
     for row in r:
        keyList = []
        for key in common_headers:
            keyList.append(row[key])
        if not keyList in  keysList:
            keysList.append(keyList)
            newRow = {}
            for key in fields:
                newRow[key] = "NaN"
            for key in headers[file.name]:
                newRow[key] = row[key]
            rowsDictionary[len(keysList)-1] = newRow
            newRow = initialRow
        else:
            for key in headers[file.name]:
                rowsDictionary[keysList.index(keyList)][key] = row[key]


with open("join.csv", mode='w') as outputCSV:

    # The next "fieldNames" are parameters that specify the amount of resources to be used in the cluster.
    writer = csv.DictWriter(outputCSV, fieldnames=fields)
    writer.writeheader()
    print("************************************************\n")
    for index in range(len(keysList)):
        writer.writerow(rowsDictionary[index])

print(keysList)
print(rowsDictionary)


#             row.update({'job-id': taskCPUStats['JobID'][0], 'max-rss': memoryStats['MaxRSS'][1], 'elapsed': taskCPUStats['Elapsed'][0], 'start': info['start'], 'finish': info['end'], 'error': error, 'status': info['status']})
#             printed.append(row['job-name'])
#             w.writerow(row)
#         else:
#             print(row['job-name'] + " ====>" + info['JobName'])
#     f.seek(0)
#     r.next() #to skip header
#     for row in r:
#         if not row['job-name'] in printed:
#             w.writerow(row)
#     move(temp.name,csvFile)

#print(result_list)
#print(headers)
