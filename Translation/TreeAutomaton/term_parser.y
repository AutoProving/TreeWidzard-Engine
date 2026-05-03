/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.
 */

%defines
%define api.prefix {term_}
%code requires {
    #include "../../TreeAutomaton/TreeAutomaton.h"
    #include <iostream>
    #include <map>
    #include <memory>
    #include <set>
    #include <vector>
}
%{
    #include <algorithm>
    #include <cmath>
    #include <iostream>
    #include <map>
    #include <memory>
    #include <set>
    #include <string>
    #include <string.h>
    #include <vector>
    #include "../../TreeAutomaton/TreeAutomaton.h"

    extern int yylex();
    extern int term_lineno;
    template<class S>
    extern void yyerror(Term<S> &term, int &result, char const* msg);
%}
%locations
%union{
     unsigned number;
     char* string;
     std::vector<unsigned>* numbers;
}
%parse-param {Term<std::string> &term}
%parse-param {int &result}

%token TR_NODE_NUM TR_COMMENT TR_NEWLINE TR_NODE_SYMBOL TR_LEFTP TR_RIGHTP TR_COMMA
%type<string> TR_NODE_SYMBOL TR_COMMENT
%type<number> TR_NODE_NUM
%type<numbers> TR_CHILDREN TR_OPTIONAL_CHILDREN

%start TR_START

%code {
    std::map<unsigned, std::shared_ptr<TermNode<std::string>>> term_label_to_node;
    std::map<unsigned, std::vector<unsigned>> term_children_by_label;
    std::map<unsigned, unsigned> term_parent_by_label;
}

%%

TR_START                   : {
                                term_label_to_node.clear();
                                term_children_by_label.clear();
                                term_parent_by_label.clear();
                             }
                             TR_LINES
                             {
                                for (const auto &entry : term_children_by_label) {
                                    auto parent_it = term_label_to_node.find(entry.first);
                                    if (parent_it == term_label_to_node.end()) {
                                        std::cout << "missing node " << entry.first << std::endl;
                                        YYERROR;
                                    }

                                    std::vector<std::shared_ptr<TermNode<std::string>>> children;
                                    for (unsigned child_label : entry.second) {
                                        auto child_it = term_label_to_node.find(child_label);
                                        if (child_it == term_label_to_node.end()) {
                                            std::cout << "child label " << child_label << " is not defined" << std::endl;
                                            YYERROR;
                                        }
                                        if (term_parent_by_label.count(child_label)) {
                                            std::cout << "node " << child_label << " has two parents "
                                                      << term_parent_by_label[child_label] << " and "
                                                      << entry.first << std::endl;
                                            YYERROR;
                                        }
                                        term_parent_by_label[child_label] = entry.first;
                                        child_it->second->setParent(parent_it->second);
                                        children.push_back(child_it->second);
                                    }
                                    parent_it->second->setChildren(children);
                                }

                                std::set<unsigned> root_candidates;
                                for (const auto &entry : term_label_to_node) {
                                    if (!term_parent_by_label.count(entry.first)) {
                                        root_candidates.insert(entry.first);
                                    }
                                }

                                if (term_label_to_node.empty()) {
                                    result = 0;
                                } else if (root_candidates.size() != 1) {
                                    std::cout << "term should have exactly one root" << std::endl;
                                    YYERROR;
                                } else {
                                    auto root = term_label_to_node[*root_candidates.begin()];
                                    term.setRoot(root);
                                    result = 0;
                                }
                             }
                           ;
TR_LINES                   : TR_LINE TR_LINES
                           |
                           ;
TR_LINE                    : TR_COMMENT TR_NEWLINE
                           | TR_NEWLINE
                           | TR_NODE_LINE TR_NEWLINE
                           | TR_NODE_LINE
                           ;
TR_NODE_LINE               : TR_NODE_NUM TR_NODE_SYMBOL TR_OPTIONAL_CHILDREN
                             {
                                if (term_label_to_node.count($1)) {
                                    std::cout << "label " << $1 << " is repeated" << std::endl;
                                    YYERROR;
                                }
                                auto node = std::make_shared<TermNode<std::string>>();
                                node->setNodeContent(std::string($2));
                                term_label_to_node[$1] = node;
                                term_children_by_label[$1] = *$3;
                                delete $3;
                             }
                           ;
TR_OPTIONAL_CHILDREN       : TR_LEFTP TR_CHILDREN TR_RIGHTP { $$ = $2; }
                           |                               { $$ = new std::vector<unsigned>; }
                           ;
TR_CHILDREN                : TR_NODE_NUM
                             {
                                $$ = new std::vector<unsigned>;
                                $$->push_back($1);
                             }
                           | TR_CHILDREN TR_COMMA TR_NODE_NUM
                             {
                                $$ = $1;
                                $$->push_back($3);
                             }
                           ;

%%
template<class S>
void yyerror(Term<S> &/*term*/, int &/*result*/, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " " <<term_lineno << std::endl;
}
