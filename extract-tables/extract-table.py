if __name__ == '__main__':
    import argparse
    import pandas as pd
    import math
    import matplotlib.pyplot as plt
    import os
    import re
    parser = argparse.ArgumentParser(description="This is a script for generating a latex table.")
    parser.add_argument('--input-file', type=argparse.FileType('r'), nargs=1, required=True, help="excel file")
    args = parser.parse_args()

    searches = ["IsomorphismBreadthFirstSearch"]
    searchFlag = [" ", "-premise"]
    property_range = range(1, 6)
    property_name = "ChromaticNumber"
    width_value = range(1, 6)
    width_name = [" pw = "]
    #comparing_key = "all-states"
    comparing_key = "max-rss"
    #comparing_key = "elapsed"
    
    file = args.input_file[0]
    data = pd.read_csv(file)


  #  number_of_search = 4
  #  

    table_file = open(os.path.splitext(file.name)[0] + "-table.txt", "w")
    table_file.write("\\begin{table}[] \n")
    table_file.write("\centering \n")
    table_file.write("\\begin{tabularx}{\\textwidth}{|p{1cm}| ")
    for search in searches:
        for f in searchFlag:
            for r in property_range:
                table_file.write("X|")
    table_file.write("}\n")
    table_file.write("\hline\n")
    table_file.write("\multicolumn{1}{|c|}{ }")
    for f in searchFlag:
        for search in searches:
            table_file.write("& \multicolumn{"+ str(len(property_range)) +"}{c|}{\\thead{"+search+f+"}}")
    table_file.write("\\\ \hline \n")
    table_file.write("\diagbox{pw}{v}")

    for f in searchFlag:
        for search in searches:
            for v in property_range:
                table_file.write(" & " + str(v))
    table_file.write("\\\ \hline \n")
    
    # fill table
    for w in width_name:
        for p in width_value:
            row = str(w+str(p)) 
            for flag in searchFlag:
                for s in searches:
                    for m in property_range:
                        l = data.loc[(data[" Search Type"] == s) & (data[property_name] == m) & (
                            data["width"] == w + str(p)) & (data["Search Options"] == flag)]
                        if len(l.index) > 0:
                            # print(l)
                            cellinfo = ""
                            if comparing_key=="elapsed":
                                m = re.match("((\d\d|\d\d)-)?(\d\d):(\d\d):(\d\d|\d)$",str(l.iloc[0][comparing_key]))
                                if m:
                                   # cellinfo = str(l.iloc[0][comparing_key])
                                   # cellinfo = str(m.group(2))+m.group(3)+m.group(4)+m.group(5)
                                   day = 0
                                   if m.group(2)==None:
                                       day = 0
                                   else:
                                       day = int(m.group(2))
                                   hour = int(m.group(3)); minute = int(m.group(4)); sec = int(m.group(5))
                                   cellinfo = str(day*24*60*60 + hour*60*60 + minute*60 + sec) 
                                else:
                                    cellinfo = "N"
                            else:
                                cellinfo = str(l.iloc[0][comparing_key])

                            if pd.isnull(l.iloc[0][comparing_key]):
                                print("comparing key is null")
                                exit()
                                
                            elif l.iloc[0]["error-output"]=="Error":
                                print("There is error")
                                print(l.iloc[0]["error"])
                                if "TIME LIMIT" in l.iloc[0]["error"]:
                                    print("TIME LIMIT error")  
                                    cellinfo = cellinfo +  "$>$"
                                    print(cellinfo)
                                elif "out-of-memory handler" in l.iloc[0]["error"]: 
                                    print("memory limit error")
                                    cellinfo+="$>$"
                                else:
                                    print("unknown error check data")
                                    exit()
                            
                            row = row + " & " + str(cellinfo)
                        
                        else:
                            row = row + " & err"
            row = row + " \\\ \hline \n"
            table_file.write(row)
        
    table_file.write("\end{tabularx}\n")
    table_file.write("\end{table}\n")
                      #  print(w+str(p), s, flag, m)
                      #  if len(l.index) > 0:
                      #      print(p, s, flag, m)
                      #  print(len(l.index))
                      #  print("--------------------------------------------------------------------------------")
                      #  if "TIME LIMIT" in str(l.iloc[0]["error"]):
                      #      row = row + " & " + str(l.iloc[0]["all-states"]) + "$>$"
                      #  elif "" in str(l.iloc[0]["error"]):
                      #      row = row + " & " + str(l.iloc[0]["all-states"])
                      #      print(l["error"])
                      #  else:
                      #      row = row + " & err"
                      #  else:
                      #  print(len(l.index))
                      #  print("--------------------------------------------------------------------------------")
#                     #   row = row + " & "
#        row = row + " \\\ \hline \n"
#        table_file.write(row)
#    table_file.write("\end{tabularx}\n")
#
                      #  print(w+str(p), s, flag, m)
                      #  if len(l.index) > 0:
                      #      print(p, s, flag, m)
                      #  print(len(l.index))
                      #  print("--------------------------------------------------------------------------------")
                      #  if "TIME LIMIT" in str(l.iloc[0]["error"]):
                      #      row = row + " & " + str(l.iloc[0]["all-states"]) + "$>$"
                      #  elif "" in str(l.iloc[0]["error"]):
                      #      row = row + " & " + str(l.iloc[0]["all-states"])
                      #      print(l["error"])
                      #  else:
                      #      row = row + " & err"
                      #  else:
                      #  print(len(l.index))
                      #  print("--------------------------------------------------------------------------------")
#                     #   row = row + " & "
#        row = row + " \\\ \hline \n"
#        table_file.write(row)
#    table_file.write("\end{tabularx}\n")
#    table_file.write("\end{table}\n")
#
    pw = "4"
#
    fig = plt.figure()
    for s in searches:
        for p in searchFlag:

            l = data.loc[
                (data["width"] == width_name[0] + pw) & (data[" Search Type"] == s) & (
                data["Search Options"] == p) & (pd.isnull(data["error"]))]
            print(l)
            plt.plot(l[property_name], l[comparing_key], label=s+p)
    #plt.plot(l2[property_name], l2['all-states'], label='bfs-premise')
    #plt.plot(l3[property_name], l3['all-states'], label='iso-bfs')
    #plt.plot(l4[property_name], l4['all-states'], label='iso-bfs-premise')
    # plt.yscale('log')
    plt.grid(True)

    plt.legend(loc='best')
    plt.show()
