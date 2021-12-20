if __name__ == '__main__':
    import argparse
    import pandas as pd
    import math

    parser = argparse.ArgumentParser(description="This is a script for generating a latex table.")
    parser.add_argument('--input-file', type=argparse.FileType('r'), nargs=1, required=True, help="excel file")
    args = parser.parse_args()

    searches = ["BreadthFirstSearch", "IsomorphismBreadthFirstSearch"]
    searchFlag = [" ","-premise"]
    value_range = range(5,8)
    path_value = range(1,6)
    file = args.input_file[0]
    data = pd.read_csv(file)
    number_of_search = 4
    # print(data.loc[(data["error-output"]=="Error" )& (data["MaxDegree"]==3)])
    table_file = open("table.txt","w")
    table_file.write("\\begin{table}[] \n")
    table_file.write("\centering \n")
    table_file.write("\\begin{tabularx}{\\textwidth}{|p{1cm}\n")
    table_file.write("\hline\n")

    for i in range(number_of_search):
        for r in value_range:
            table_file.write("|X")
        if i != 4:
            table_file.write("|")
    table_file.write("}\n")
    table_file.write("\multicolumn{1}{|c}{ } & \multicolumn{"+str(len(value_range))+"}{c}{\\thead{BFS}} &  \multicolumn{"+str(len(value_range))+"}{c}{\\thead{"
                     "Iso-BFS}}&  \multicolumn{"+str(len(value_range))+"}{c}{\\thead{BFS-Premise}} &  \multicolumn{"+str(len(value_range))+"}{c|}{\\thead{"
                     "Iso-BFS-Premie}}\\\ \hline \n")
    table_file.write("\diagbox{pw}{v}")

    for i in range(number_of_search):
        for v in value_range:
            table_file.write(" & "+str(v))
    table_file.write("\\\ \hline \n")
    for p in path_value:
        row = str(p)
        for flag in searchFlag:
            for s in searches:
                for m in value_range:
                    l = data.loc[(data[" Search Type"] == s) & (data["MaxDegree"] == m) & (
                                data["width"] == " pw = " + str(p)) & (data["Search Options"] == flag)]
                    if len(l.index) > 0:
                        print( p, s,flag, m)
                        print(len(l.index))
                        print("--------------------------------------------------------------------------------")
                        if "TIME LIMIT" in str(l.iloc[0]["error"]):
                            row = row + " & " + str(math.isqrt(l.iloc[0]["all-states"]))+"$>$"
                        elif  "" in str(l.iloc[0]["error"]):
                            row = row + " & " + str(math.isqrt(l.iloc[0]["all-states"]))
                        else:
                            row = row + " & err"
                    else:
                            print( p, s,flag, m)
                            print(len(l.index))
                            print("--------------------------------------------------------------------------------")
                            row = row + " & "
        row = row + " \\\ \hline \n"
        table_file.write(row)
    table_file.write("\end{tabularx}\n")
    table_file.write("\end{table}\n")
        # for index, row in data.iterrows():
        #     print(index,row["job-name"])
