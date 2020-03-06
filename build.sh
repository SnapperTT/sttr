mkdir temp
tools/lzz src/*.lzz -o temp/
#cat temp/sttr.tpl >> temp/sttr.cpp
#rm -f sttr.tpl
#sed -i 's/constexpr_/constexpr /g' temp/sttr* 
#sed -i 's/export_void/export void/g' temp/sttr.*
perl -0777 -i -pe 's/[\s\n\t]+{/ {/g' temp/*.cpp
perl -0777 -i -pe 's/\t\t/\t/g' temp/*.cpp     
perl -0777 -i -pe 's/[\s\n\t]+{/ {/g' temp/*.h
perl -0777 -i -pe 's/\t\t/\t/g' temp/*.h
cat temp/sttr_visitor.cpp temp/sttr.cpp > sttr.cpp
cp -rf src/type_to_string.hpp .
cp -rf temp/sttr.h temp/sttr_visitor.h .
g++ -g example.cpp -o example

