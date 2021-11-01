
echo Running tests
for i in 1 2 3 4 5 6
do
echo "Test #$i"
./testAss1 $i > /dev/null
done

gcov -j AVLTree.c
gcov -j Flightdb.c

rm *.gcda
rm *.gcno

