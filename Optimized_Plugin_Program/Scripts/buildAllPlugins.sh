#!/bin/bash
p=$PWD
rm -r $PWD/../DynamicPlugins/*.so
for eachfile in $(find $PWD/../DynamicCores -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  g++ -fPIC -shared $p/../Kernel/WitnessSet.cpp $p/../Kernel/Witness.cpp $p/../Kernel/DynamicCore.cpp $eachfile -o $p/../DynamicPlugins/$name.so
done
rm -r $PWD/../SearchPlugins/*.so
for eachfile in $(find $PWD/../Search -type f -name "*.cpp"); do
  name=$(basename "$eachfile" .cpp)
  echo $name
  echo $eachfile
  echo $p
  g++ -fPIC -shared $p/../Kernel/DynamicKernel.cpp $p/../Kernel/DynamicCore.cpp $p/../Conjecture/Conjecture.cpp $p/../Kernel/SearchStrategy.cpp $eachfile -o $p/../SearchPlugins/$name.so
done
mkdir $PWD/auxfiles
