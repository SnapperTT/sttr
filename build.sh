mkdir temp
tools/lzz src/sttr.lzz -t -o temp/
cat temp/sttr.tpl >> temp/sttr.cpp
#rm -f sttr.tpl
perl -0777 -i -pe 's/[\s\n\t]+{/ {/g' temp/sttr.cpp
perl -0777 -i -pe 's/\t\t/\t/g' temp/sttr.cpp     
perl -0777 -i -pe 's/[\s\n\t]+{/ {/g' temp/sttr.h
perl -0777 -i -pe 's/\t\t/\t/g' temp/sttr.h
cp -rf temp/sttr.cpp temp/sttr.h .
g++ -g example.cpp -o example

