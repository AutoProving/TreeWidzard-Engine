if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description="This is a script for generating a command for NAUTY to check whether two given graphs are isomorphic.")
    parser.add_argument('--input-file', type=argparse.FileType('r'), nargs="+",required=True, help="two graph input files should be passed.")
    args = parser.parse_args()
    nauty_text = open("nauty_command.txt","w")
    nauty_text.write("$=1 At\n")
    nauty_text.write("c -a V=0\n")
    for f in args.input_file:
        
        nauty_text.write(f.read())
        nauty_text.write(".\n")
        nauty_text.write("x @\n")
    nauty_text.write("##")    
    
