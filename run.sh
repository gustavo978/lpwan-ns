a=100
for i in 1 2 3 4
do
  echo "$a"
  mkdir output
  ./waf --run "examples/lpwan/experiment"
  mkdir $a.nodos.$i
  mv Dl* $a.nodos.$i
  mv Ul* $a.nodos.$i
  mv output/ $a.nodos.$i/output/
done
