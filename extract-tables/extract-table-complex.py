if __name__ == '__main__':
    import argparse
    import pandas as pd
    import math
    import matplotlib.pyplot as plt
    import os

    parser = argparse.ArgumentParser(description="This is a script for generating a latex table.")
    parser.add_argument('--input-file', type=argparse.FileType('r'), nargs=1, required=True, help="excel file")
    args = parser.parse_args()

    searches = ["BreadthFirstSearch", "IsomorphismBreadthFirstSearch"]
    searchFlag = [" ", "-premise"]
    value_range = range(5, 8)
    path_value = range(1, 6)
    file = args.input_file[0]
    data = pd.read_csv(file)
    number_of_search = 4
    property_name = "MaxDegree"
    # print(data.loc[(data["error-output"]=="Error" )& (data["MaxDegree"]==3)])
    # print(os.path.splitext(file.name)[0])

    table_file = open(os.path.splitext(file.name)[0] + "-table.txt", "w")
    table_file.write("\\begin{table}[] \n")
    table_file.write("\centering \n")
    table_file.write("\\begin{tabularx}{\\textwidth}{|p{1cm}\n")

    for i in range(number_of_search):
        for r in value_range:
            table_file.write("|X")
        if i != 4:
            table_file.write("|")
    table_file.write("}\n")
    table_file.write("\hline\n")
    table_file.write(
        "\multicolumn{1}{|c}{ } & \multicolumn{" + str(len(value_range)) + "}{c}{\\thead{BFS}} &  \multicolumn{" + str(
            len(value_range)) + "}{c}{\\thead{"
                                "Iso-BFS}}&  \multicolumn{" + str(
            len(value_range)) + "}{c}{\\thead{BFS-Premise}} &  \multicolumn{" + str(len(value_range)) + "}{c|}{\\thead{"
                                                                                                        "Iso-BFS-Premie}}\\\ \hline \n")
    table_file.write("\diagbox{pw}{v}")

    for i in range(number_of_search):
      for v in value_range:
            table_file.write(" & " + str(v))
    table_file.write("\\\ \hline \n")
    for p in path_value:
        row = str(p)
        for flag in searchFlag:
            for s in searches:
                for m in value_range:
                    l = data.loc[(data[" Search Type"] == s) & (data[property_name] == m) & (
                            data["width"] == " pw = " + str(p)) & (data["Search Options"] == flag) & (
                                     pd.notnull(data["all-states"]))]
                    if len(l.index) > 0:
                        print(p, s, flag, m)
                        print(len(l.index))
                        print("--------------------------------------------------------------------------------")
                        if "TIME LIMIT" in str(l.iloc[0]["error"]):
                            row = row + " & " + str(math.isqrt(int(l.iloc[0]["all-states"]))) + "$>$"
                            # row = row + " & " + str(l.iloc[0]["all-states"]) + "$>$"
                        elif "MEMORY LIMIT" in str(l.iloc[0]["error"]):
                            row = row + " & " + str(math.isqrt(int(l.iloc[0]["all-states"]))) + "$>$"
                            # row = row + " & " + str(l.iloc[0]["all-states"]) + "$>$"
                        elif pd.isnull(l.iloc[0]["error"]):
                            row = row + " & " + str(math.isqrt(int(l.iloc[0]["all-states"])))
                            # row = row + " & " + str(l.iloc[0]["all-states"])
                        else:
                            row = row + " & c"
                    else:
                        print(p, s, flag, m)
                        print(len(l.index))
                        print("--------------------------------------------------------------------------------")
                        row = row + " & "
        row = row + " \\\ \hline \n"
        table_file.write(row)
    table_file.write("\end{tabularx}\n")
    table_file.write("\end{table}\n")

    pw = "3"

    l1 = data.loc[
        (data["width"] == " pw = " + pw) & (data[" Search Type"] == "BreadthFirstSearch") & (
                data["Search Options"] == " ") & (pd.isnull(data["error"]))]
    l2 = data.loc[(data["width"] == " pw = " + pw) & (data[" Search Type"] == "BreadthFirstSearch") & (
            data["Search Options"] == "-premise") & (pd.isnull(data["error"]))]
    l3 = data.loc[(data["width"] == " pw = " + pw) & (data[" Search Type"] == "IsomorphismBreadthFirstSearch") & (
            data["Search Options"] == " ") & (pd.isnull(data["error"]))]
    l4 = data.loc[(data["width"] == " pw = " + pw) & (data[" Search Type"] == "IsomorphismBreadthFirstSearch") & (
            data["Search Options"] == "-premise") & (pd.isnull(data["error"]))]

    # print(l1)
    fig = plt.figure()
    plt.plot(l1[property_name], l1['all-states'], label='bfs')
    plt.plot(l2[property_name], l2['all-states'], label='bfs-premise')
    plt.plot(l3[property_name], l3['all-states'], label='iso-bfs')
    plt.plot(l4[property_name], l4['all-states'], label='iso-bfs-premise')
    # plt.yscale('log')
    plt.grid(True)

    plt.legend(loc='best')
    plt.show()
