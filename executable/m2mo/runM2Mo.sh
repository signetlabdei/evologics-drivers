if [ $# -lt 8 ]
then
	echo "1 - nodeID "
	echo "2 - destID "
	echo "3 - period "
  echo "4 - hours "
  echo "5 - minutes"
  echo "6 - duration"
  echo "7 - ip"
  echo "8 - port"
else
	mkdir $1
  ./m2mo $1 $7 $8 >> outEvol$1_topology$2.log &
  echo "mainExp($1,$2,$3,$4,$5,00,$6)"| matlab 2>&1 | tee -a outHandShake$1_topology$2.log
fi
killall m2mo