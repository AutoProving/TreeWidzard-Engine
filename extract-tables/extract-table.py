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

    #searches = ["IsomorphismBreadthFirstSearch"]
    searches = ["BreadthFirstSearch"]
    searchesName = {"IsomorphismBreadthFirstSearch":"ISO-BFS", "BreadthFirstSearch":"BFS"}
    searchFlag = [" ", "-premise"]
    property_range = range(2, 7)
    #property_name = "ChromaticNumber"
    #property_name = "CliqueNumberSimpleGraphs"
    property_name = "MaxDegree"
    width_value = range(1, 6)
    width_name = [" pw = "]
    #comparing_key = "all-states"
    #comparing_key = "max-rss"
    comparing_key = "elapsed"
    
    file = args.input_file[0]
    data = pd.read_csv(file)


  #  number_of_search = 4
  #  
    base=os.path.basename(file.name)

    table_file = open(os.path.dirname(file.name)+"/"+comparing_key+"-"+os.path.splitext(base)[0] + "-table.txt", "w")
    table_file.write("\\begin{table}[H] \n")
    table_file.write("\centering \n")
    table_file.write("\\begin{tabularx}{\\textwidth}{|p{1.2cm}| ")
    for search in searches:
        for f in searchFlag:
            for r in property_range:
                table_file.write("X|")
    table_file.write("}\n")
    table_file.write("\hline\n")
    table_file.write("\multicolumn{1}{|c|}{ }")
    for f in searchFlag:
        for search in searches:
            table_file.write("& \multicolumn{ "+ str(len(property_range)) +" }{c|}{\\thead{\\footnotesize{ "+searchesName[search]+f+" } }}")
    table_file.write("\\\ \hline \n")
    table_file.write("\diagbox{\\footnotesize pw}{ \\footnotesize v}")

    for f in searchFlag:
        for search in searches:
            for v in property_range:
                table_file.write(" & " + "\\tiny{"+str(v)+"}")
    table_file.write("\\\ \hline \n")
    
    # fill table
    for w in width_name:
        for p in width_value:
            row = "\\tiny{ " +str(w+str(p))+ " }"
            for flag in searchFlag:
                for s in searches:
                    for m in property_range:
                        l = data.loc[(data[" Search Type"] == s) & (data[property_name] == m) & (
                            data["width"] == w + str(p)) & (data["Search Options"] == flag)]
                        if len(l.index) > 0:
                            # print(l)
                            cellinfo = ""
                            if comparing_key=="elapsed":
                                m = re.match("((\d\d|\d)-)?(\d\d):(\d\d):(\d\d|\d)$",str(l.iloc[0][comparing_key]))
                                if m:
                                   # cellinfo = str(l.iloc[0][comparing_key])
                                   # cellinfo = str(m.group(2))+m.group(3)+m.group(4)+m.group(5)
                                   day = 0
                                   if m.group(2)==None:
                                       day = 0
                                   else:
                                       day = int(m.group(2))
                                   hour = int(m.group(3)); minute = int(m.group(4)); sec = int(m.group(5))
                                   cellinfo = str(int((day*24*60*60 + hour*60*60 + minute*60 + sec)/60)) 
                                else:
                                    cellinfo = "N"
                                    print("error in regular expression 1")
                                    print(str(l.iloc[0][comparing_key]))
                                    print(m)
                                    print(l.iloc[0]["job-name"])
                                    exit()

                            else:
                                #cellinfo = str(int(round(math.sqrt(l.iloc[0][comparing_key]),0)))
                                cellinfo = str(int(l.iloc[0][comparing_key]))
                            if pd.isnull(l.iloc[0][comparing_key]):
                                print("comparing key is null")
                                exit()
                                
                            elif l.iloc[0]["error-output"]=="Error":
                                print("There is error")
                                print(l.iloc[0]["error"])
                                if "TIME LIMIT" in l.iloc[0]["error"]:
                                    print("TIME LIMIT error")  
                                    #cellinfo = cellinfo +  " $>$"
                                    cellinfo = "\\textcolor{red}{ " +cellinfo+"}"
                                    print(cellinfo)
                                elif "out-of-memory handler" in l.iloc[0]["error"]: 
                                    print("memory limit error")
                                    #cellinfo+="$>$"
                                    cellinfo = "\\textcolor{red}{ " +cellinfo+"}"
                                else:
                                    print("unknown error check data")
                                    cellinfo = "\\textcolor{red}{ " + cellinfo+"}"
                                    #exit()
                            
                            row = row + " & " + "\\tiny{"+str(cellinfo)+"}"
                        
                        else:
                            row = row + " & err"
            row = row + " \\\ \hline \n"
            table_file.write(row)
        
    table_file.write("\end{tabularx}\n")
    table_file.write("\caption{"+ base+"-" + comparing_key+"}\n")
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
    pw = "3"
#
    ls = ["-","--","-.",".."]
    ss = ["X","o","s","p"]
    k = 0
    op = 0.1
  
    for s in searches:
        for p in searchFlag: 
            l = data.loc[
                (data["width"] == width_name[0] + pw) & (data[" Search Type"] == s) & (
                data["Search Options"] == p)]
           # l = data.loc[
           #     (data["width"] == width_name[0] + pw) & (data[" Search Type"] == s) & (
           #     data["Search Options"] == p) & (pd.isnull(data["error"]))]
            x = []
            y = []
            nx = []
            ny = []
            print(l[comparing_key])
            for r in range(len(l)):
                if l.iloc[r][property_name] in property_range:
                    #print(l.iloc[r][comparing_key])
                    yp = l.iloc[r][comparing_key] 
                    xp = l.iloc[r][property_name]
                    if comparing_key == "max-rss":
                        m = re.match("(\d*)K?$",l.iloc[r][comparing_key])
                        if m:
                            yp = int(m.group(1))
                        else:
                            print("regular expression error")
                            exit()
                    elif comparing_key=="elapsed":
                        m = re.match("((\d\d|\d)-)?(\d\d):(\d\d):(\d\d|\d)$",str(l.iloc[r][comparing_key]))
                        if m:
                            day = 0
                            if m.group(2)==None:
                                day = 0
                            else:
                                day = int(m.group(2))
                            hour = int(m.group(3)); minute = int(m.group(4)); sec = int(m.group(5))
                            yp = int((day*24*60*60 + hour*60*60 + minute*60 + sec)/60)
                            print(day,hour,minute,sec,yp,s,p,l.iloc[r][comparing_key],l.iloc[r][property_name])
                        else:
                            print("error in regular expression 1")
                            print(str(l.iloc[r][comparing_key]))
                            print(m)
                            print(l.iloc[r]["job-name"])
                            break
        
                    if not pd.isnull(l.iloc[r]["error"]):
                        nx.append(xp)
                        ny.append(yp)
                    x.append(xp)
                    y.append(yp)
            plt.plot(x,y, marker='',label=searchesName[s]+p,alpha=1-op,linestyle=ls[k],linewidth =8-3*k)
            plt.scatter(nx,ny, marker=ss[k],zorder = 10-k,s=100+100*k, label=searchesName[s]+p+" not finished", alpha=1-op)
            k+=1
            op+=.3
      
    #plt.plot(l2[property_name], l2['all-states'], label='bfs-premise')
    #plt.plot(l3[property_name], l3['all-states'], label='iso-bfs')
    #plt.plot(l4[property_name], l4['all-states'], label='iso-bfs-premise')
    #plt.yscale('log')
    plt.yscale('symlog')
  #  plt.plot(nx,ny, marker = 'X')
    plt.grid(True)
    plt.ylabel(comparing_key)
    plt.xlabel(property_name)
    plt.legend(loc='best')
    #plt.legend(loc=1,bbox_to_anchor=(1,1))
    
    plt.savefig(os.path.dirname(file.name)+"/"+comparing_key+"-"+os.path.splitext(base)[0]+"-fig.png")
    plt.show()
