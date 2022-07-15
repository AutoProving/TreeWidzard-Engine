#!/bin/bash
p=$PWD
rm -r $PWD/../DynamicPlugins/*.so

for eachfile in $(find $PWD/../DynamicCores -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  g++ -fPIC -shared $p/../Kernel/WitnessSet.cpp $p/../Kernel/Witness.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so
  #g++ -fsanitize=thread -g3 -fPIC -shared $p/../Kernel/WitnessSet.cpp $p/../Kernel/Witness.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so &
done


for eachfile in $(find $PWD/../../../treewidzard-CoresTemp -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  g++ -fPIC -shared $p/../Kernel/WitnessSet.cpp $p/../Kernel/Witness.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so
  #g++ -fsanitize=thread -g3 -fPIC -shared $p/../Kernel/WitnessSet.cpp $p/../Kernel/Witness.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so &
done
wait
