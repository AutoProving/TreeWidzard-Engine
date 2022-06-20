#!/bin/bash
p=$PWD
rm -r $PWD/../SearchPlugins/*.so

for eachfile in $(find $PWD/../Search -type f -name "*.cpp"); do
#for eachfile in $(find $PWD/../Search/BFS/ParallelBreadthFirstSearch.cpp -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  echo $p
  g++ -pthread -fPIC -shared $p/../Kernel/DynamicKernel.cpp $p/../Kernel/DynamicCore.cpp $p/../Conjecture/Conjecture.cpp $p/../Kernel/SearchStrategy.cpp $eachfile -o $p/../SearchPlugins/$name.so
  #g++ -pthread -fsanitize=thread -g3 -fPIC -shared $p/../Kernel/DynamicKernel.cpp $p/../Kernel/DynamicCore.cpp $p/../Conjecture/Conjecture.cpp $p/../Kernel/SearchStrategy.cpp $eachfile -o $p/../SearchPlugins/$name.so &
done

wait

mkdir $PWD/auxfiles
