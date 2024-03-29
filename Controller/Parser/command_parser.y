%defines
%define api.prefix {command_}
%code requires {
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
}
%{
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
    #include <algorithm>
    #include "../SearchController.h"
    #include "../../Kernel/Flags.h"
    #include "../ParseController.h"
    #include "../AuxiliaryController.hpp"

    // this function will be generated
    // using flex
    extern int yylex();
    extern int command_lineno;
    extern void yyerror(int &result, std::string &width_type, int &width_value,char const* msg);
    Flags flags;

  %}
%locations
%union{
     float number;
     char* string;
}
%parse-param {int &result}
%parse-param {std::string &width_type}
%parse-param {int &width_value}
%token command_newline command_search_signature command_print_state_flag command_print_loop_flag command_string command_help command_end
        command_parse_signature command_parse_pace command_parse_itd command_term_signature command_print_state_tree command_random_signature
        command_number command_premise command_no_bfs_dag command_nthreads command_pw command_tw command_equal command_print_directed_bipartite_graph
        command_write_files
%type<string> command_newline command_search_signature command_print_state_flag command_print_loop_flag
              command_string command_input_file command_search_strategy command_help command_end command_random_signature
              command_parse_signature command_parse_pace command_parse_itd command_term_signature command_print_state_tree command_premise command_no_bfs_dag command_nthreads
              command_pw command_tw command_equal command_print_directed_bipartite_graph command_write_files
%type<number> command_number
%start command_start
%glr-parser

%%

command_start       : command_search
                    | command_help command_end {show_manual();}
                    | command_parse
                    | command_term
                    ;
command_search      : command_search_signature command_width command_flags
                      command_search_strategy  command_input_file command_end { Width width;
										width.set_name(width_type);
										width.set_value(width_value);
                      								SearchController search($5, $4, flags, width);
                                                                                search.action();
                                                                                }
		    | command_search_signature command_width command_flags command_random
                                            command_search_strategy  command_input_file command_end { Width width;
													width.set_name(width_type);
													width.set_value(width_value);
                                            							      	SearchController search($6, $5,flags,width);
                                                                                                      	search.action();
                                                                                                     }

                    ;
command_width 	    : command_pw  command_equal command_number {width_type = "path_width"; if($3<0){std::cout<< "width value should be not negative" << std::endl; YYERROR;} width_value=$3;}
		    | command_tw command_equal command_number {width_type = "tree_width"; if($3<0){std::cout<< "width value should be not negative" << std::endl; YYERROR;} width_value=$3;}
command_random	    : command_random_signature command_number command_number command_number command_number {
													    if($3+$4>1){std::cout<<"sum of the probalities is bigger than 1"<<std::endl;
													    YYERROR;}
													    flags.add_flag("seedValue", $2);
													    flags.add_flag("probAddVertex", $3);
													    flags.add_flag("probForgetVertex", $4);
													    flags.add_flag("numberOfIterations", $5);
												        	}
		    ;
command_flags       : command_print_state_flag command_flags {flags.add_flag("PrintStates", 1);}
                    | command_print_loop_flag command_flags  {flags.add_flag("LoopTime", 1);}
                    | command_print_state_tree command_flags {flags.add_flag("StateTree", 1);}
                    | command_premise command_flags {flags.add_flag("Premise", 1);}
                    | command_no_bfs_dag command_flags {flags.add_flag("NoBFSDAG", 1);}
                    | command_nthreads command_number command_flags { if ($2<1){std::cout<< "Number of threads to use should be at least 1." << std::endl; YYERROR; } flags.add_flag("NThreads", $2);}
                    | command_print_directed_bipartite_graph command_flags {flags.add_flag("PrintDirectedBipartiteGraphNAUTY", 1);}
                    | command_write_files command_flags {flags.add_flag("WriteToFiles", 1);}
                    |                                        {}
                    ;

command_input_file  : command_string { $$=$1;}
                    ;
command_search_strategy : command_string { $$=$1;}
                    ;
command_parse       : command_parse_signature command_parse_pace command_flags command_input_file command_input_file command_input_file
                      command_end{ ParseController parsePACE(flags, $4); parsePACE.parse_pace($5, $6); }
                    | command_parse_signature command_parse_itd command_flags command_input_file command_input_file command_end
                        { ParseController parsePACE(flags, $4); parsePACE.parse_itd($5);}
                    ;
command_term        : command_term_signature command_input_file command_end{ParseController parseTest(flags,$2); parseTest.test_term();}

%%

void yyerror(int &, std::string &, int &, char const*){
  //std::cerr<<"Syntax Error: "<< msg << " on line " <<command_lineno << std::endl;
  std::cout<<"Wrong number of inputs. Please execute treewidzard --help for more information."<<std::endl;
  // error printing  disabled, it is handeled in main.cpp
}
