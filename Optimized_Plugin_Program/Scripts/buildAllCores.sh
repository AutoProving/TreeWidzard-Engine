#!/bin/bash
p=$PWD
rm -r $PWD/../DynamicPlugins/*.so

for eachfile in $(find $PWD/../DynamicCores -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  g++ -fPIC -shared -DCORE_FILE_NAME=$name $p/../Kernel/WitnessSet.cpp $p/../Kernel/WitnessBase.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so
  #g++ -fsanitize=thread -g3 -fPIC -shared -DCORE_FILE_NAME=$name $p/../Kernel/WitnessSet.cpp $p/../Kernel/WitnessBase.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so &
done

wait
